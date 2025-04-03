#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

int LED_Pin = 2;  // GPIO2 for ESP32 onboard LED

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const char* mqtt_server = "test.mosquitto.org";

// Global variable to store the received state
String state = "";

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, 6);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("arduinoClient_45678675432")) {
      Serial.println("connected");
      mqttClient.subscribe("Seamk_testi"); // Subscribe after connecting
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// MQTT callback function (must be void)
void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    
    Serial.print("Message: ");
    state = ""; // Clear the previous state
    for (int i = 0; i < length; i++) {
        state += (char)payload[i];  // Convert payload to String
    }
    Serial.println(state);
    Serial.println("-----------------------");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_Pin, OUTPUT);
  delay(2000);
  
  Serial.println("Initializing WiFi...");
  initWiFi();
  
  Serial.println("Setting up MQTT...");
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);
  
  reconnect();  // Connect to MQTT
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Control LED based on received message
  if (state == "on") {
    //Serial.println("LED ON");
    digitalWrite(LED_Pin, HIGH);
  } else if (state == "off") {
    //Serial.println("LED OFF");
    digitalWrite(LED_Pin, LOW);
  }
}
