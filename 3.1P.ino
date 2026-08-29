#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>
#include <PubSubClient.h>

// Wi-Fi details
const char* WIFI_SSID = "Galaxy A56 5G FF25";   // Enter the complete Wi-Fi name
const char* WIFI_PASSWORD = "1234567890";

// HiveMQ details
const char* MQTT_SERVER = "4d76df5677af4efa9f59b00ee2770ca1.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;

const char* MQTT_USERNAME = "SIT210";
const char* MQTT_PASSWORD = "ChitkaraUniversity";

const char* MQTT_TOPIC = "Terrarium/Sunlight";

// Create connections
WiFiSSLClient wifiClient;
PubSubClient mqttClient(wifiClient);
BH1750 lightSensor;


void setup() {
  Serial.begin(9600);
  delay(2000);

  Wire.begin();
  lightSensor.begin();

  connectWiFi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  connectMQTT();
}


void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  float light = lightSensor.readLightLevel();

  Serial.print("Light: ");
  Serial.print(light);
  Serial.println(" lx");

  String lightMessage = String(light, 2);

  if (mqttClient.publish(MQTT_TOPIC, lightMessage.c_str())) {
    Serial.println("Published to HiveMQ!");
  } else {
    Serial.println("Publish failed!");
  }

  delay(2000);
}


void connectWiFi() {

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    delay(5000);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting to HiveMQ...");

    String clientID = "Nano33IoT-";
    clientID += String(random(0xffff), HEX);

    if (mqttClient.connect(
          clientID.c_str(),
          MQTT_USERNAME,
          MQTT_PASSWORD)) {

      Serial.println("connected!");

    } else {

      Serial.print("Connection failed, state = ");
      Serial.println(mqttClient.state());

      delay(5000);
    }
  }
}