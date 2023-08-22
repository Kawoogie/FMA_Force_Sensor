# FMA_Force_Sensor

Code to run the FMA series of micro force sensors by Honeywell.

Example of use:
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
   // force_sensor.set_zero();
 
   while (true) {

       force_value = force_sensor.get_force();
       temp_value = force_sensor.get_temp();
      
       serial.printf("%.2f, %.1f\n", force_value, temp_value);
       
       ThisThread::sleep_for(10ms);
   }
}

```
