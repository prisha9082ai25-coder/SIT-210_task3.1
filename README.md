# Terrarium Sunlight Trigger

This project is a sunlight monitoring and notification system built using an Arduino Nano 33 IoT and a BH light sensor. The sensor measures light intensity in lux, and the Arduino publishes the readings to a HiveMQ MQTT broker through the `Terrarium/Sunlight` topic.

Node-RED receives and processes the sensor readings using MQTT and a Function node. The Debug node is used to monitor the data, while the Email node sends notifications when the sunlight condition changes.

## Components Used

- Arduino Nano 33 IoT
- BH Light Sensor
- Wi-Fi
- HiveMQ Cloud MQTT
- Node-RED
- Email Notification

## System Flow

BH Sensor → Arduino Nano 33 IoT → Wi-Fi → HiveMQ MQTT → Node-RED → Function → Email
