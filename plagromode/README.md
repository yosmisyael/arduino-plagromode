# arduino-plagromode
This is the main code program of PLAGROMODE (Plant Growth Monitoring Device). The abbreviation describes the decive usage.

## description
This code is written for an ESP32 microcontroller. It reads data from a soil temperature sensor (DS18B20), an air temperature and humidity sensor (DHT22), and a soil moisture sensor (soil capacitive sensor). The data was then displayed on the monitor (I2C 1602 LCD) and also streamed to the IoT platform (Blynk) in order to make the microntroller accessible from the internet.

This device has two mode, manual and automatic mode. By default, mode is set to manual, which needed to turn on or off the water pump manually through Blynk. In automatic mode, the device will automatically turn the water pump on or off based on soil moisture condition. Automatic mode use average soil moisture value to decide whether soil need to be watered or not. Those other two sensor, DHT22 and DS18B20 will give insight about environment condition inside plant box.

## devices
  - ESP32
  - LCD Display 1602 I2C
  - DHT22
  - DS18B20
  - Soil capacitative sensor
  - Water pump 5V DC
  - Relay module
