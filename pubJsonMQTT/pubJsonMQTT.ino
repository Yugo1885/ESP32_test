/* DHT11 sensor generates temperature and humidity, than publishes json form of both date to MQTT client */

#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"

#define MQTT_HOST IPAddress(192, 168, xxx, xx)
#define MQTT_PORT 1883
//temperature and humidity mqtt topic
#define MQTT_PUB_DHT "esp32/dht11"

#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
const long interval = 10000;  //interval at which to publish

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  //Serial.print("  packetId: ");
  //Serial.println(packetId);
}

void setup() {
  dht.begin();
  Serial.begin(115200);
  Serial.println();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");

  connectToWifi();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;

    if(isnan(h) || isnan(t)){
    Serial.println(F("Failed to read from DHT sensor"));
    return;
    }
    StaticJsonDocument<64> doc;
    doc["sensor"] = "dht";
    doc["temp"] = t;
    doc["humd"] = h;

    char out[128];
    serializeJson(doc, out);
    
    // Publish an MQTT message on topic esp32/dht11
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_DHT, 1, true, out);                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: ", MQTT_PUB_DHT);
    Serial.println(packetIdPub1);
    Serial.printf("Message: ");
    serializeJson(doc, Serial);
    Serial.println();
  }
}
