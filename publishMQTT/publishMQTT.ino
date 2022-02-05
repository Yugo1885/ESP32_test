/* 原作者:https://randomnerdtutorials.com/esp32-mqtt-publish-dht11-dht22-arduino/#more-95552 */
#include <WiFi.h>
#include <DHT.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define DHTPIN 5
#define DHTTYPE DHT11
const int potPin = 4; //可變電阻腳位

//mqtt broker
#define mqtt_host IPAddress(140,128,110,156)
#define mqtt_port 1883

#define mqtt_pub_temp "esp/dht/temperature"
#define mqtt_pub_hum "esp/dht/humidity"
#define mqtt_pub_pot "esp/pot"

#define ssid  "your_ssid"
#define password  "your_password"

DHT dht(DHTPIN,DHTTYPE);

float temp;
float hum;
int pot_val = 0;

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
  Serial.printf("[WiFi-event] event: %d\n", event);
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
  Serial.print(sessionPresent);
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
  mqttClient.setServer(mqtt_host, mqtt_port);
  
  //如果broker要求驗證(username and password)
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  
  connectToWifi();
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    pot_val =analogRead(potPin);
    int pot=map(pot_val,0,1023,0,225);
    if(isnan(temp)||isnan(hum)){ //isnan = is NOT A NUMBER which return true when it is not a number 
      Serial.println(F("無偵測到DHT sensor!"));
      return;
    }
    
    //publish an mqtt message on topic esp/dht/temperature
    uint16_t packetIdPub1 = mqttClient.publish(mqtt_pub_temp, 1, true, String(temp).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", mqtt_pub_temp, packetIdPub1);
    Serial.printf("Message: %.2f \n", temp);

    // Publish an MQTT message on topic esp32/dht/humidity
    uint16_t packetIdPub2 = mqttClient.publish(mqtt_pub_hum, 1, true, String(hum).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", mqtt_pub_hum, packetIdPub2);
    Serial.printf("Message: %.2f \n", hum);
    
    // Publish an MQTT message on topic esp32/pot
    uint16_t packetIdPub3 = mqttClient.publish(mqtt_pub_pot, 1, true, String(pot).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", mqtt_pub_pot, packetIdPub3);
    Serial.printf("Message: %.2f \n", pot);
    
  }
                                             
}
  
