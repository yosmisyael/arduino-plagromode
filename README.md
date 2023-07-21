# iot-plant-monitoring
This code is used in IoT device for plant monitoring and automatic watering.

## function
This IoT device can monitor temperature, humidity and soil moisture through several sensors. The data from the sensors is displayed on the monitor and also sent to the IOT platform, I use Blynk in this project.
Data from soil moisture sensor readings is used to determine whether plants need to be watered or not. Watering can be done automatically by the device in automatic mode.

## devices
  - ESP32
  - LCD Display 1602 I2C
  - DHT22
  - DS18B20
  - Soil capacitative sensor
  - Water pump 5V DC
  - Relay module (I use double channel for water pump and lamp)
  - Lamp (Optional)

     
