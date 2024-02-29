#ifndef FMA_FORCE_SENSOR_H
#define FMA_FORCE_SENSOR_H
/**
 *  Library to get force and temperature readings from the FMA series of
 *  micro force sensors from Honeywell.
 * 
 *  This code is written to get data from the I2C protocol, not the SPI protocol
 *  
 *  This was written for use with Mbed OS 6.16.0 on a MAX32630FTHR
 * 
 * Example of use:
 * @code
 * #include "mbed.h"
 * #include "max32630fthr.h"
 * #include "fma_force_sensor.h"
 * #include "USBSerial.h"  // For communication via the onboard USB port
 * 
 * // Enable FTHR
 * MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3); // Enable J1-pin2, 3.3V and set GPIO to 3.3v
 * 
 * // Start serial communication
 * USBSerial serial;
 * 
 * // Start I2C bus on the correct pins
 * I2C i2c_bus(P3_4, P3_5); // SDA, SCL
 * 
 * // Create the Sensor_Raw object
 * FMA_Force_Sensor force_sensor(i2c_bus);
 * 
 * int main()
 * {
 *    
 *    i2c_bus.frequency(400000);
 *    float force_value = 0.0;
 *    float temp_value = 0.0;
 *    // force_sensor.set_zero();
 *
 *    while (true) {
 *
 *        force_value = force_sensor.get_force();
 *        temp_value = force_sensor.get_temp();
 *       
 *        serial.printf("%.2f, %.1f\n", force_value, temp_value);
 *        
 *        ThisThread::sleep_for(10ms);
 *    }
 * }
 * @endcode
 * 
*/
#include "mbed.h"

class FMA_Force_Sensor {
    public:
    /** Constructor
     * @param Data data pin for the I2C communication
     * @param Clock clock pin for the I2C communication
     * @param max_value max range of force reading value
     * @param transfer_max upper limit of transfer function percentage.
    */
    FMA_Force_Sensor(I2C &i2cbus, int max_value = 25, int transfer_max = 80);
    int get_force(float &force_out);
    int get_temp(float &temp_out);
    float set_zero(void);
    void set_address(uint8_t new_address);
    float get_zero(void); 
    
    private:
    I2C &_sh_i2c;
    int _max_range;
    int _transfer_value;
    int _device_address;
    float _zero_value;
    int _get_force_raw(int &force_val);
    int _get_temp_raw(int &temp_val);
    void _calibration_delay(void);
    void _set_zero_value(float zero_offset);

};

#endif