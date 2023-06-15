#ifndef FMA_FORCE_SENSOR_H
#define FMA_FORCE_SENSOR_H
/**
 *  Library to get force and temperature readings from the FMA series of
 *  micro force sensors from Honeywell.
 * 
 *  This code is written to get data from the I2C protocol, not the SPI protocl
 *  
 *  This was written for use with Mbed OS 6.16.0 on a MAX32630FTHR
 * 
 * Example of use:
 * @code
 * #include "mbed.h"
 * #include "max32664.h"
 * #include "USBSerial.h"  // For communication via the onboard USB port
 * 
 * 
 * // Start serial communication
 * USBSerial serial;
 * 
 * // Create the Sensor_Raw object
 * FMA_Force_Sensor force_sensor;
 * 
 * int main()
 * {   
 *     // Initialize the sensor hub with default values
 *     sensor_hub.init_sh_raw();
 *     
 *     // Set the data interrogation rate to 50 Hz.
 *     sensor_hub.set_sample_rate(50);
 *     
 *     // Set the drive current for all 3 LEDs 
 *     sensor_hub.set_led_current(0x12);
 *  
 *     // Variables to hold the ppg data
 *     int red_ppg, green_ppg, ir_ppg;
 * 
 *     while (true) {
 *       // Read the data at a rate of 50 Hz and output the information via serial
 *       sensor_hub.read_sh_fifo(&red_ppg, &green_ppg, &ir_ppg);
 *       serial.printf("%d,%d,%d,\n\r", red_ppg, green_ppg, ir_ppg);
 *       ThisThread::sleep_for(20ms);
    }
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
    FMA_Force_Sensor(PinName Data = P5_7, PinName Clock = P6_0, int max_value = 25, int transfer_max = 80);
    void get_force(float* force);
    void get_temp(float* temperature);
    void set_zero(void);
    void set_address(int new_address);
    

    private:
    I2C _sh_i2c;
    int _max_range;
    int _transfer_value;
    int _device_address;
    int _zero_value;
    void _get_force_raw(uint16_t* force_raw);
    void _get_temp_raw(uint16_t* temp_raw);
    void _calibration_delay(void);

};

#endif