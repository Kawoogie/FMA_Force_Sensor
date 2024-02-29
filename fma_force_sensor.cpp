#include "fma_force_sensor.h"
#include "mbed.h"


// Count values for calculating the calibrated force value
#define FORCE_MAX_COUNT                                    16384.0
#define FORCE_90_COUNT                                     14745.6
#define FORCE_80_COUNT                                     13107.2
#define FORCE_20_COUNT                                     3276.8
#define FORCE_10_COUNT                                     1638.4

// Bit masks for decoding the data output
#define FORCE_STATE_BIT_MASK                               0x03


#define CALIBRATION_DELAY                                  50ms

// Default address for the sensor
const int addr8bit = 0x28 << 1; // 8bit I2C address, 0x80


/*****************************************************************************/
// Constructor
/*****************************************************************************/
/*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*/
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

int FMA_Force_Sensor::get_force(float &force_out){
    uint16_t force_raw = 0;
    float force_uncompensated = 0.0;
    int status;

    status = _get_force_raw(force_raw);

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

int FMA_Force_Sensor::get_temp(float &temp_out){
    uint16_t temp_raw = 0;
    // float temp = 0.0;
    int status;
    // Get raw values
    status = _get_temp_raw(temp_raw);

    // Temperature calibration for Celcius
    temp_out = ((float(temp_raw) / 2047) * 200) - 50.0;
    
    return status;
}

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
        force_val = 0;
        return status;
    }
}

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
        temp_raw <<= 8;
        temp_raw |= response[3];
        temp_raw >>= 5;

        temp_val = temp_raw & 0x07FF;
        return status;

    }
    else{
        temp_val = 0;
        return status;
    }

}

void FMA_Force_Sensor::set_address(uint8_t new_address){
    int addr_shifted = new_address << 1;
    _device_address = addr_shifted;
}

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

float FMA_Force_Sensor::get_zero(void){
    return _zero_value;
}


void FMA_Force_Sensor::_set_zero_value(float zero_offset){
    _zero_value = zero_offset;
}

void FMA_Force_Sensor::_calibration_delay (void) {
    ThisThread::sleep_for(CALIBRATION_DELAY);
}