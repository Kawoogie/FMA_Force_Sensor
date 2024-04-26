#include "fma_force_sensor.h"
#include "mbed.h"


// Count values for calculating the calibrated force value
#define FORCE_MAX_COUNT                                    16384.0
#define FORCE_90_COUNT                                     14745.6
#define FORCE_80_COUNT                                     13107.2
#define FORCE_20_COUNT                                     3276.8
#define FORCE_10_COUNT                                     1638.4

// Maximum force range values
#define MAX_VALUE_COUNT                                    3
const int MAX_VALUES[] = {5, 15,25};

// Bit masks for decoding the data output
#define FORCE_STATE_BIT_MASK                               0x03


#define CALIBRATION_DELAY                                  50ms

// Default address for the sensor
const int addr8bit = 0x28 << 1; // 8bit I2C address, 0x80


/** 
 * @brief Constructor
 * 
 * @param I2C I2C object for the I2C communication
 * @param max_value max range of force sensor. Typically 5, 15, or 25.
 * @param transfer_max upper limit of transfer function percentage.
*/
FMA_Force_Sensor::FMA_Force_Sensor(
                            I2C &i2cbus,
                            int max_value,
                            int transfer_max
                            ):
                            _sh_i2c(i2cbus)
    {
        // Set values for max_value and transfer_max assignments
        _max_range = max_value;
        _transfer_value = transfer_max;
        _zero_value = 0.0;

        // Address of the sensor
        _device_address = addr8bit;
    }

/**
 * @brief Get the force reading value from the sensor
 * 
 * @param[out] force_out float value of the sensor in Newtons
 * @returns 0 on success, other value if failure
 * if an I2C read error, will be 1 with a force_out value of -99. 
*/
int FMA_Force_Sensor::get_force(float &force_out){
    int force_raw = 0;
    float force_uncompensated = 0.0;
    int status;

    status = _get_force_raw(force_raw);

    if (force_raw == 99){
        force_out = 99;
        return status;
    }

    else {
    // Calibrate the force value depending on the transfer function range.
    if (_transfer_value == 80){
        force_uncompensated = ((float(force_raw) - FORCE_20_COUNT) / (FORCE_80_COUNT - FORCE_20_COUNT)) * _max_range;
    }
    else{
        force_uncompensated = ((float(force_raw) - FORCE_10_COUNT) / (FORCE_90_COUNT - FORCE_10_COUNT)) * _max_range;
    }

    // Remove the zero offset from the value
    force_out = force_uncompensated - _zero_value;

    return status;
    } 
}

/**
 * @brief Get the temperature of the force sensor package.
 * 
 * @param[out] temp_out float value of the package temp.
 * @returns 0 on success, other value if failure
 * if an I2C read error, will be 1 with a force_out value of -99. 
*/
int FMA_Force_Sensor::get_temp(float &temp_out){
    int temp_raw = 0;
    // float temp = 0.0;
    int status;
    // Get raw values
    status = _get_temp_raw(temp_raw);


    if (temp_raw == 99){
        temp_out = temp_raw;
    }
    else{
    // Temperature calibration for Celcius
    temp_out = ((float(temp_raw) / 2047) * 200) - 50.0;
    }

    return status;
}

/**
 * @brief Function to change the I2C address used to access the sensor.
 * The default value used is 0x28.
 * 
 * @param[in] new_address New uint8_t value to use as sensor address.
 * @returns 0 on success, other value if failure. 
*/
void FMA_Force_Sensor::set_address(uint8_t new_address){
    int addr_shifted = new_address << 1;
    _device_address = addr_shifted;
}

/**
 * @brief Function to set the zero read value of the sensor force measurements.
 * Averages over 10 readings of the sensor.
 * Sets the internal private function _zero_value.
 * 
 * @returns float value of the new sensor zero value. 
*/
float FMA_Force_Sensor::set_zero(void){
    // uint16_t force_val;
    // uint8_t status;
    uint8_t n_cnt;
    float sum = 0.0;
    float force_reading = 0.0;
    float offset = 0.0;
    int status;
        
    for ( n_cnt = 0; n_cnt < 10; n_cnt++ ) {
        
        status = get_force(force_reading);
        
        if (!status){
            sum += force_reading;
        }

        _calibration_delay();
    }
    
    offset = (sum / 10.0);
    _set_zero_value(offset);

    return offset;
}

/**
 * @brief Function to get the value used as the zero offset value
 * 
 * @returns float value of the sensor zero value. 
*/
float FMA_Force_Sensor::get_zero(void){
    return _zero_value;
}

/**
 * @brief Function to set the max force value of the sensor
 * 
 * @returns int 0 if success 
*/
int FMA_Force_Sensor::set_max_value(int new_max){
    int status = 1;

    // Check to see if the new value is a valid value
    // Only assign it if it is a valid value
    // Set status to 0 to indicate success
    for (int i = 0; i < MAX_VALUE_COUNT; i++){
        if (MAX_VALUES[i] == new_max){
            _max_range = new_max;
            status = 0;
        }
    }

    return status;
}

/**
 * @brief Function to set the sensor max value from two bool inputs. This
 * function is used to set the max reading value of the sensor using pins
 * grounded on a microprocessor. Set the respective bool false to configure
 * the sensor. the newton_5 setting has precedence over the newton_15 setting.
 * If neither pin is false, then 25 N is selected.
 * 
 * @param newton_5 bool set false for 5 N max scale
 * @param newton_15 bool set false for 15 N max scale
 * 
 * @returns int 0 if a value is sucessfully set 
*/
int FMA_Force_Sensor::max_config(bool newton_5, bool newton_15){
    int status = 1;

    if (!newton_5){
        status = set_max_value(5);
    }

    elif (!newton_15){
        status = set_max_value(15);
    }

    else {
        status = set_max_value(25);
    }

    return status;
}

/**
 * @brief Function to return the max value set for the device in Newtons
 * 
 * @returns int value of the sensor max value in Newtons. 
*/
int FMA_Force_Sensor::max_value(void){
    return _max_range;
}

/**
 * @brief Helper function to get the raw reading of the sensor
 * force value before any conversions.
 * 
 * @param[out] force_val in value of the raw force reading.
 * @returns 0 on success, other value if failure. 
*/
int FMA_Force_Sensor::_get_force_raw(int &force_val){
    char response[4];
    int status;
    int force_raw;

    status = _sh_i2c.read(_device_address, response, 4);

    if (!status){
    
        // Get the status of the force read
        status = ( response[0] >> 6 ) & FORCE_STATE_BIT_MASK;

        // Parse the force data from the response
        force_raw = response[0];
        force_raw <<= 8;
        force_raw |= response[1];

        force_val = force_raw & 0x3FFF; // Apply mask
        return status;
    }
    else{
        force_val = -99;
        return status;
    }
}

/**
 * @brief Helper function to get the raw reading of the sensor
 * temperature value before any conversions.
 * 
 * @param[out] temp_val in value of the raw temperature reading.
 * @returns 0 on success, other value if failure. 
*/
int FMA_Force_Sensor::_get_temp_raw(int &temp_val){
    char response[4];
    int status;
    int temp_raw;

    status = _sh_i2c.read(_device_address, response, 4);

    if (!status){
        // Get the status of the force read
        status = ( response[0] >> 6 ) & FORCE_STATE_BIT_MASK;

        // Parse the temp data from the response
        temp_raw = response[2];
        temp_raw <<= 3;
        temp_raw |= (response[3] >> 5);

        temp_val = temp_raw;
        return status;

    }
    else{
        temp_val = -99;
        return status;
    }

}

/**
 * @brief Helper function to delay the code to allow the force sensor to settle
 * before taking calibration measurements.
 * 
*/
void FMA_Force_Sensor::_calibration_delay (void) {
    ThisThread::sleep_for(CALIBRATION_DELAY);
}

/**
 * @brief Helper function to set the private value _zero_value. Used in
 * setting the zero offset of the sensor force reading values.
 * 
 * @param zero_offset float value of the sensor zero value. 
*/
void FMA_Force_Sensor::_set_zero_value(float zero_offset){
    _zero_value = zero_offset;
}