# FMA_Force_Sensor

Code to run the FMA series of micro force sensors by Honeywell.


Example of use for code 1.1 versions:
```
#include "mbed.h"
#include "max32630fthr.h"
#include "fma_force_sensor.h"
#include "USBSerial.h"

// Enable FTHR
MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3); // Enable J1-pin2, 3.3V and set GPIO to 3.3v

// Start serial communication
USBSerial serial;

// Start I2C bus on the correct pins
I2C i2c_bus(P3_4, P3_5); // SDA, SCL

// Initialize the force sensor and associate it with the I2C bus
FMA_Force_Sensor force_sensor(i2c_bus);

// main() runs in its own thread in the OS
int main()
{
    // Set the I2C bus speed
    i2c_bus.frequency(100000);
    // Set the address for the device
    force_sensor.set_address(40);

    float force_value = 0.0;
    float temp_value = 0.0;
    int force_status;
    int temp_status;
    
    ThisThread::sleep_for((1000ms));
    
    // Set the zero value for the force readings
    serial.printf("Setting Zero Value \n");
    force_sensor.set_zero();

    while (true) {

        // Get the force value
        force_status = force_sensor.get_force(force_value);
        ThisThread::sleep_for(20ms);        
        // Get the temperature
        temp_status = force_sensor.get_temp(temp_value);
        
        // If there are no errors, display the values
        if (!force_status && !temp_status){
        serial.printf("%.3f, %.2f\n", force_value, temp_value);
        }
        // Display the errors along with the returned value.
        // A value of 99 with a status of 1 indicates an I2C read error
        else{
            serial.printf("Error: Force: %.3f, Status: %d\n", force_value, force_status);
            serial.printf("Error: Temp: %.2f, Status %d\n", temp_value, temp_status);
        }

        ThisThread::sleep_for(100ms);
    }
}

```


Example of use for code 1.0 versions:
```
   #include "mbed.h"
   #include "max32630fthr.h"
   #include "fma_force_sensor.h"
   #include "USBSerial.h"  // For communication via the onboard USB port
   // Enable FTHR
   MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3); // Enable J1-pin2, 3.3V and set GPIO to 3.3v
 
   // Start serial communication
   USBSerial serial;

   // Start I2C bus on the correct pins
   I2C i2c_bus(P3_4, P3_5); // SDA, SCL
 
   // Create the Sensor_Raw object
   FMA_Force_Sensor force_sensor(i2c_bus);
 
   int main()
   {
    
   i2c_bus.frequency(400000);
   float force_value = 0.0;
   float temp_value = 0.0;
   ThisThread::sleep_for((1000ms)); // Time for sensor to initialize
   force_sensor.set_zero();
 
   while (true) {

       force_value = force_sensor.get_force();
       temp_value = force_sensor.get_temp();
      
       serial.printf("%.2f, %.1f\n", force_value, temp_value);
       
       ThisThread::sleep_for(10ms);
   }
}

```
