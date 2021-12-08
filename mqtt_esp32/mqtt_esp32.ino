#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <AsyncMqttClient.h>

#define DHTPIN 5
#define DHTTYPE DHT11
#define ldrPin 6
//mqtt broker
#define mqtt_host IPAddress(192, 168, 1 ,xxx)
#define mqtt_port 1883

#define mqtt_pub_temp "esp/dht/temperature"
#define mqtt_pub_hum "esp/dht/humidity"
#define mqtt_pub_ldr "esp/ldr"

const char* ssid = wifi_ssid;
const char* password = wifi_password;

DHT dht(DHTPIN,DHTTYPE);

float temp;
float humd;
int ldr;

AsyncMqttClient mqttClient;

//connects ESP32 to router
void connectToWifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
}
//connects ESP32 to mqtt broker
void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const

//MQTT 連線後一系列的發送與接收 demo
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT");
  Serial.print("Session present: ");
  Serial.print(seesionPresent);
}
//MQTT disconnect
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT");
  if(WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer,0);
  }
}
//MQTT subscribe                   
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}
//MQTT unsubscribe
void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}                   

void setup() {
  Serial.begin(115200);
  Serial.println();
}

void loop() {
}
  

