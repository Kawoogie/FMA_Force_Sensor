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
 * #include "USBSerial.h"
 * 
 * // Enable FTHR
 * MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3); // Enable J1-pin2, 3.3V and set GPIO to 3.3v

 * // Start serial communication
 * USBSerial serial;
 * 
 * // Start I2C bus on the correct pins
 * I2C i2c_bus(P3_4, P3_5); // SDA, SCL
 * 
 * // Initialize the force sensor and associate it with the I2C bus
 * FMA_Force_Sensor force_sensor(i2c_bus);
 * 
 * // main() runs in its own thread in the OS
 * int main()
 * {
 *     // Set the I2C bus speed
 *     i2c_bus.frequency(100000);
 *     // Set the address for the device
 *     force_sensor.set_address(40);
 * 
 *     float force_value = 0.0;
 *     float temp_value = 0.0;
 *     int force_status;
 *     int temp_status;
 *     
 *     ThisThread::sleep_for((1000ms));
    
 *     // Set the zero value for the force readings
 *     serial.printf("Setting Zero Value \n");
 *     force_sensor.set_zero();
 * 
 *     while (true) {
 * 
 *         // Get the force value
 *         force_status = force_sensor.get_force(force_value);
 *         ThisThread::sleep_for(20ms);        
 *         // Get the temperature
 *         temp_status = force_sensor.get_temp(temp_value);
 *         
 *         // If there are no errors, display the values
 *         if (!force_status && !temp_status){
 *         serial.printf("%.3f, %.2f\n", force_value, temp_value);
 *         }
 *         // Display the errors along with the returned value.
 *         // A value of 99 with a status of 1 indicates an I2C read error
 *         else{
 *             serial.printf("Error: Force: %.3f, Status: %d\n", force_value, force_status);
 *             serial.printf("Error: Temp: %.2f, Status %d\n", temp_value, temp_status);
 *         }
 * 
 *         ThisThread::sleep_for(100ms);
 *     }
 * }
 * 
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