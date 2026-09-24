#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>
#include <PubSubClient.h>

// LIBRARY PURPOSES

// WiFiNINA.h
// Used to connect the Arduino Nano 33 IoT to a Wi-Fi network.
//
// Wire.h
// Provides I2C communication between the Arduino and the BH1750
// light sensor.
//
// BH1750.h
// Provides functions for reading the light intensity from the
// BH1750 sensor in lux.
//
// PubSubClient.h
// Provides MQTT communication so the Arduino can publish
// sensor readings to the HiveMQ MQTT broker.

// WI-FI DETAILS

// WIFI_SSID stores the name of the Wi-Fi network.
// WIFI_PASSWORD stores the password required to connect.
//
// These values are used by the connectWiFi() function.
//
// NOTE: The SSID and password are sensitive credentials and
// should normally not be shared publicly.

const char* WIFI_SSID = "Galaxy A56 5G FF25";   // Enter the complete Wi-Fi name
const char* WIFI_PASSWORD = "1234567890";

// HIVEMQ MQTT BROKER DETAILS

// MQTT_SERVER contains the address of the HiveMQ Cloud broker.
//
// MQTT_PORT is set to 8883.
// Port 8883 is commonly used for MQTT communication over
// TLS/SSL, which provides an encrypted connection.

const char* MQTT_SERVER = "4d76df5677af4efa9f59b00ee2770ca1.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;

// HIVEMQ LOGIN DETAILS

// These credentials are used by the Arduino when connecting
// to the HiveMQ Cloud MQTT broker.
//
// MQTT_USERNAME contains the HiveMQ username.
// MQTT_PASSWORD contains the corresponding password.
//
// The broker uses these details to authenticate the Arduino
// before allowing it to communicate using MQTT.

const char* MQTT_USERNAME = "SIT210";
const char* MQTT_PASSWORD = "ChitkaraUniversity";

// MQTT TOPIC

// MQTT_TOPIC defines the MQTT topic where the light readings
// will be published.
//
// Node-RED or another MQTT client can subscribe to this same
// topic to receive the sunlight readings.
//
// In this project, the topic is:
// Terrarium/Sunlight

const char* MQTT_TOPIC = "Terrarium/Sunlight";

// CREATE CONNECTION OBJECTS

// WiFiSSLClient creates a secure Wi-Fi client connection.
// It is used because the HiveMQ broker is using port 8883,
// which is an SSL/TLS MQTT connection.
//
// PubSubClient uses the Wi-Fi client to communicate with
// the MQTT broker.
//
// BH1750 lightSensor creates an object representing the
// BH1750 light sensor. This object is used to read the
// light intensity in lux.

WiFiSSLClient wifiClient;
PubSubClient mqttClient(wifiClient);
BH1750 lightSensor;

// SETUP FUNCTION

// setup() runs once when the Arduino starts or resets.
//
// It is used to initialise:
// 1. Serial communication
// 2. I2C communication
// 3. BH1750 light sensor
// 4. Wi-Fi connection
// 5. MQTT server
// 6. MQTT connection

void setup() {

  // Start serial communication at 9600 bits per second.
  // This allows us to see messages and sensor readings
  // in the Arduino Serial Monitor.
  Serial.begin(9600);

  // Wait for 2 seconds after starting the serial connection.
  // This gives the Arduino and Serial Monitor some time
  // to initialise.
  delay(2000);

  // INITIALISE I2C AND BH1750
  // Start the I2C communication.
  // The BH1750 communicates with the Arduino using I2C.
  Wire.begin();

  // Initialise the BH1750 light sensor.
  // After this, the Arduino can request light measurements
  // from the sensor.
  lightSensor.begin();

  // CONNECT TO WI-FI
  // Call the connectWiFi() function.
  // The function keeps trying until the Arduino successfully
  // connects to the specified Wi-Fi network.
  connectWiFi();

  // CONFIGURE MQTT SERVER

  // Tell the PubSubClient which MQTT broker and port
  // should be used for communication.
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  // CONNECT TO HIVEMQ

  // Attempt to establish an MQTT connection with HiveMQ.
  // The username and password are used inside connectMQTT().
  connectMQTT();
}
// MAIN LOOP
// loop() runs continuously after setup() has finished.
//
// During every loop:
// 1. Wi-Fi connection is checked.
// 2. MQTT connection is checked.
// 3. MQTT communication is maintained.
// 4. Light intensity is read.
// 5. The reading is displayed on Serial Monitor.
// 6. The reading is converted into a message.
// 7. The reading is published to HiveMQ.
// 8. The Arduino waits for 2 seconds.

void loop() {

  // CHECK WI-FI CONNECTION

  // Check whether the Arduino is still connected to Wi-Fi.
  //
  // WL_CONNECTED means that the Arduino currently has
  // a valid Wi-Fi connection.
  //
  // If the connection is lost, connectWiFi() is called to
  // reconnect to the network.

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // CHECK MQTT CONNECTION

  // Check whether the Arduino is still connected to HiveMQ.
  //
  // If the MQTT connection has been lost, connectMQTT()
  // attempts to establish the connection again.

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  // MAINTAIN MQTT CONNECTION
  // mqttClient.loop() allows the MQTT client to process
  // incoming and outgoing MQTT communication.
  //
  // It also helps maintain the MQTT connection with HiveMQ.

  mqttClient.loop();

  // READ LIGHT LEVEL

  // Read the current light intensity from the BH1750 sensor.
  //
  // The reading is stored in a float variable because light
  // intensity can contain decimal values.
  //
  // For example:
  // 3521.67 lux
  //
  // The unit returned by readLightLevel() is lux (lx).

  float light = lightSensor.readLightLevel();

  // DISPLAY LIGHT LEVEL

  // Print the text "Light: " to the Serial Monitor.
  Serial.print("Light: ");

  // Print the measured light value.
  Serial.print(light);

  // Print the unit "lx" and move to a new line.
  Serial.println(" lx");

  // CONVERT LIGHT VALUE TO MQTT MESSAGE

  // Convert the floating-point light value into a String.
  //
  // The value 2 means that two decimal places are included
  // in the resulting message.
  //
  // Example:
  // A sensor value of 3521.674 becomes "3521.67".
  //
  // MQTT publishes data as a message, so the numerical
  // sensor value needs to be converted into a string.

  String lightMessage = String(light, 2);

  // PUBLISH LIGHT READING TO HIVEMQ
  // Publish the light reading to the MQTT topic:
  // Terrarium/Sunlight
  //
  // lightMessage.c_str() converts the Arduino String into
  // the character format required by PubSubClient.
  //
  // publish() returns true when the message is successfully
  // published and false if the publication fails.

  if (mqttClient.publish(MQTT_TOPIC, lightMessage.c_str())) {

    // Display a success message if the reading was published.
    Serial.println("Published to HiveMQ!");

  } else {

    // Display an error message if publishing failed.
    Serial.println("Publish failed!");
  }

  // WAIT BEFORE NEXT READING

  // Wait for 2 seconds before taking and publishing the
  // next light reading.
  //
  // This means the sensor data is approximately published
  // every 2 seconds.

  delay(2000);
}

// FUNCTION: CONNECT TO WI-FI

// This function establishes a Wi-Fi connection.
//
// It continuously checks the Wi-Fi status.
// If the Arduino is not connected, it calls WiFi.begin()
// using the provided SSID and password.
//
// The while loop continues until WL_CONNECTED is returned.

void connectWiFi() {


  // Display a message before starting the connection process.
  Serial.print("Connecting to Wi-Fi");

  // KEEP TRYING UNTIL WI-FI IS CONNECTED

  // The while loop continues running while the Arduino
  // is not connected to the Wi-Fi network.

  while (WiFi.status() != WL_CONNECTED) {


    // Start or retry the Wi-Fi connection using the
    // configured network name and password.
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);


    // Wait 5 seconds before checking the connection again.
    // This gives the Wi-Fi module time to establish a
    // connection.

    delay(5000);


    // Print a dot to show that the Arduino is still trying
    // to connect.
    Serial.print(".");
  }

  // WI-FI CONNECTED

  // Print a new line and display a confirmation message.
  Serial.println();
  Serial.println("Wi-Fi connected!");


  // Display the IP address assigned to the Arduino by
  // the Wi-Fi network.
  //
  // This can be useful for identifying the Arduino on
  // the local network.

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// FUNCTION: CONNECT TO HIVEMQ

// This function establishes the MQTT connection between
// the Arduino and the HiveMQ Cloud broker.
//
// It keeps trying until mqttClient.connected() returns true.
//
// A unique client ID is generated for the Arduino so that
// HiveMQ can identify this MQTT client.

void connectMQTT() {

  // KEEP TRYING UNTIL MQTT IS CONNECTED
  // The loop continues until the Arduino successfully
  // connects to the HiveMQ MQTT broker.

  while (!mqttClient.connected()) {


    // Display a message indicating that an MQTT connection
    // attempt is being made.
    Serial.print("Connecting to HiveMQ...");

    // CREATE A UNIQUE MQTT CLIENT ID
   
    // Start the client ID with a descriptive name.
    //
    // A random hexadecimal number is then added to the end.
    // This helps create a unique ID for the MQTT connection.

    String clientID = "Nano33IoT-";
    clientID += String(random(0xffff), HEX);

    // ATTEMPT MQTT CONNECTION
    // mqttClient.connect() attempts to authenticate the
    // Arduino with the HiveMQ broker.
    //
    // Parameters:
    // 1. clientID.c_str() -> unique MQTT client ID
    // 2. MQTT_USERNAME    -> HiveMQ username
    // 3. MQTT_PASSWORD    -> HiveMQ password
    //
    // If the connection succeeds, it returns true.
    // If it fails, it returns false.

    if (mqttClient.connect(
          clientID.c_str(),
          MQTT_USERNAME,
          MQTT_PASSWORD)) {


      // Display a successful connection message.
      Serial.println("connected!");


    } else {

      // MQTT CONNECTION FAILED
    
      // Display the MQTT connection state/error code.
      //
      // mqttClient.state() helps identify why the connection
      // attempt was unsuccessful.

      Serial.print("Connection failed, state = ");
      Serial.println(mqttClient.state());


      // Wait 5 seconds before attempting the MQTT connection
      // again. This prevents the Arduino from continuously
      // attempting to connect without a delay.

      delay(5000);
    }
  }
}
