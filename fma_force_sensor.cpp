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


#define CALIBRATION_DELAY                                  10ms

// Default address for the sensor
const int addr8bit = 0x28 << 1; // 8bit I2C address, 0x80


/*****************************************************************************/
// Constructor
/*****************************************************************************/
/*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*/
FMA_Force_Sensor::FMA_Force_Sensor(
                            PinName Data,
                            PinName Clock,
                            int max_value,
                            int transfer_max
                            ):
                            _sh_i2c(Data, Clock)
    {
        // Set up I2C communication

        // Set values for max_value and transfer_max assignments
        _max_range = max_value;
        _transfer_value = transfer_max;
        _device_address = addr8bit;
        _zero_value = 0.0;
        // Address of the sensor

    }

float FMA_Force_Sensor::get_force(void){
    uint16_t force_raw = 0;
    float force_uncompensated = 0;
    float force;

    _get_force_raw(force_raw);


    // Calibrate the force value depending on the transfer function range.
    if (_transfer_value == 80){
        force_uncompensated = ((float(force_raw) - FORCE_20_COUNT) / (FORCE_80_COUNT - FORCE_20_COUNT)) * _max_range;
    }
    else{
        force_uncompensated = ((float(force_raw) - FORCE_10_COUNT) / (FORCE_90_COUNT - FORCE_10_COUNT)) * _max_range;
    }

    // Remove the zero offset from the value
    force = force_uncompensated - _zero_value;

    return force;
    
}

float FMA_Force_Sensor::get_temp(void){
    uint16_t temp_raw = 0;
    float temp = 0.0;
    
    // Get raw values
    temp_raw = _get_temp_raw(&temp_raw);

    // Temperature calibration for Celcius
    temp = ((float(temp) / 2047) * 200) - 50.0;
    
    return temp;
}



void FMA_Force_Sensor::_get_force_raw(uint16_t force_raw){
    char response[4];
    int status;

    _sh_i2c.read(_device_address, response, 4);

    // Get the status of the force read
    status = ( response[0] >> 6 ) & FORCE_STATE_BIT_MASK;

    // Parse the force data from the response
    force_raw = response[0];
    force_raw <<= 8;
    force_raw |= response[1];
    force_raw &= 0x3FFF; // Apply mask

}

void FMA_Force_Sensor::_get_temp_raw(uint16_t temp_raw){
    char response[4];
    int status;

    _sh_i2c.read(_device_address, response, 4);

    // Get the status of the force read
    status = ( response[0] >> 6 ) & FORCE_STATE_BIT_MASK;

    // Parse the temp data from the response
    temp_raw = response[2];
    temp_raw <<= 8;
    temp_raw |= response[3];
    temp_raw >>= 5;
    temp_raw &= 0x07FF;


}


void FMA_Force_Sensor::set_address(int new_address){
    _device_address = new_address;
}

void FMA_Force_Sensor::set_zero(void){
    // uint16_t force_val;
    // uint8_t status;
    uint8_t n_cnt;
    float sum;
    float force = 0.0;
    
    sum = 0;
    
    for ( n_cnt = 0; n_cnt < 10; n_cnt++ ) {
        get_force(force);
        
        sum += force;
        
        _calibration_delay();
    }
    
    _zero_value = ( sum / 10.0 );
}


void FMA_Force_Sensor::_calibration_delay (void) {
    ThisThread::sleep_for(CALIBRATION_DELAY);
}