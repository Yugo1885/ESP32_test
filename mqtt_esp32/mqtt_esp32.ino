#include <Wire.h>
#include <DHT.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
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

//建立AsyncMqttClien物件mqttClient來處理mqtt用戶端與timers當mqtt代理人和router斷線時可重新連接
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
//建立輔助計時器變數，每10秒發布讀取值
unsigned long previousMillis = 0; //stores last time temperature was published
const long interval = 10000; //interval at which to publish sensor readings

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

//此函數處理WiFi事件。如果成功連接至broker和router，則打印esp32位址;反之失去連接則開始timer並reconnect
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n",, event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi連線");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi斷線");
      xTimerStop(mqttReconnectTimer, 0); //關閉定時器
      xTimerStart(wifiReconnectTimer, 0); //開啟定時器
      break;
  }
}

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

//當發布訊息給mqtt topic時作用，顯示packet id 
void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print(" packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  dht.begin();
  
  //以防斷線，建立mqtt與wifi定時器用來reconnect
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  //callback function, 當esp32連上WiFi將執行此函數，並打印詳細訊息
  WiFi.onEvent(WiFiEvent);
  
  //callback function
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  
  //如果broker要求驗證(username and password)
  mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  
  connectToWiFi();
}

void loop() {
}
  

