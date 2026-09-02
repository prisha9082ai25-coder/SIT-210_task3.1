#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi Credentials
char ssid[] = "Galaxy A56 5G FF25";
char password[] = "1234567890";

// MQTT Broker
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;

// MQTT Topics
const char* waveTopic = "ES/Wave";
const char* patTopic = "ES/Pat";

// Pins
const int trigPin = 2;
const int echoPin = 3;
const int led1 = 4;
const int led2 = 5;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Function Prototypes
void connectMQTT();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  Serial.print("Connecting to WiFi");

  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }

  Serial.println();
  Serial.println("WiFi Connected");

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  connectMQTT();
}

void loop() {

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.0343 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Wave Detection
  if (distance >= 15 && distance <= 30) {

    Serial.println("Wave Detected");

    mqttClient.publish(waveTopic, "Pravesh");

    delay(2000);
  }

  // Pat Detection
  else if (distance > 0 && distance < 10) {

    Serial.println("Pat Detected");

    mqttClient.publish(patTopic, "Pravesh");

    delay(2000);
  }

  delay(100);
}

// MQTT Connection
void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.println("Connecting to MQTT...");

    String clientID = "NanoClient-";
    clientID += String(random(1000, 9999));

    if (mqttClient.connect(clientID.c_str())) {

      Serial.println("MQTT Connected");

      mqttClient.subscribe(waveTopic);
      mqttClient.subscribe(patTopic);

      Serial.println("Subscribed to ES/Wave");
      Serial.println("Subscribed to ES/Pat");

    } else {

      Serial.print("MQTT Failed. State: ");
      Serial.println(mqttClient.state());

      delay(3000);
    }
  }
}

// MQTT Callback
void callback(char* topic, byte* payload, unsigned int length) {

  String receivedMessage = "";

  for (unsigned int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(receivedMessage);

  if (String(topic) == waveTopic) {

    Serial.println("Turning LEDs ON");

    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }

  else if (String(topic) == patTopic) {

    Serial.println("Turning LEDs OFF");

    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
}