<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>
<!-- -->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Kawoogie/FMA_Force_Sensor">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">FMA Force Sensor</h3>

  <p align="center">
    This is a library to run the FMA series of micro force sensors by Honeywell via I2C. It is written to work with a MAX32630FTHR, but it could easily be adapted for other microprocessors.
    <br />
    <a href="https://github.com/Kawoogie/FMA_Force_Sensor"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/Kawoogie/FMA_Force_Sensor">View Demo</a>
    ·
    <a href="https://github.com/Kawoogie/FMA_Force_Sensor/issues">Report Bug</a>
    ·
    <a href="https://github.com/Kawoogie/FMA_Force_Sensor/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>    
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

This project is a custom library written to operate the FMA series of micro force sensors from Honeywell. This particular library was written for the I2C version of the chip, and does not include functionality for the SPI version.

This library allows reading of the measured force value in Newtons as well as the package tempeature in Celsius. 

<p align="right">(<a href="#readme-top">back to top</a>)</p>




<!-- GETTING STARTED -->
## Getting Started

This library should be added to your favourite IDE and imported. See the Usage section for how to implement.

### Prerequisites

The code only requires standard C++ functions along with I2C.


### Installation

The .h and .cpp files need to be downloaded and added to their own folder in the libraries you are using.

If using Keil Studio Cloud, the Github URL for this repository needs to be added to the libraries for the project.



<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage


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

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/Kawoogie/FMA_Force_Sensor/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->

## License


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact
Project Link: [https://github.com/Kawoogie/FMA_Force_Sensor](https://github.com/Kawoogie/FMA_Force_Sensor)

<p align="right">(<a href="#readme-top">back to top</a>)</p>






<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/Kawoogie/FMA_Force_Sensor.svg?style=for-the-badge
[contributors-url]: https://github.com/Kawoogie/FMA_Force_Sensor/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/Kawoogie/FMA_Force_Sensor.svg?style=for-the-badge
[forks-url]: https://github.com/Kawoogie/FMA_Force_Sensor/network/members
[stars-shield]: https://img.shields.io/github/stars/Kawoogie/FMA_Force_Sensor.svg?style=for-the-badge
[stars-url]: https://github.com/Kawoogie/FMA_Force_Sensor/stargazers
[issues-shield]: https://img.shields.io/github/issues/Kawoogie/FMA_Force_Sensor.svg?style=for-the-badge
[issues-url]: https://github.com/Kawoogie/FMA_Force_Sensor/issues
[license-shield]: https://img.shields.io/github/license/Kawoogie/FMA_Force_Sensor.svg?style=for-the-badge
[license-url]: https://github.com/Kawoogie/FMA_Force_Sensor/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/lee-sikstrom-a6472a113
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com



