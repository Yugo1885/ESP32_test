#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#define outTopic "esp/test"

const char* ssid = "your_ssid";
const char* password = "your_password";

const char* mqtt_server = "your_mqtt_server_ip";
const int mqtt_port = 1883;
const char* mqtt_user = "your_mqtt_username";
const char* mqtt_password = "your_mqtt_password";

WiFiClient espClient;
PubSubClient client(espClient); //建立PubSubClient客戶端

void setup(){
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(F("連接WiFi中..."));
  }
  Serial.println(F("已連上WiFi"));
  Serial.print("IP位址: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_server, mqtt_port);
  while(!client.connected()){
    Serial.println(F("連接MQTT中..."));
    if(client.connect("esp32Client", mqtt_user, mqtt_password)){ //client.connect("random clientID", mqtt_user, mqtt_password)
      Serial.println(F("已連上MQTT"));
    }else{
      Serial.print(F("錯誤狀態: "));
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop(){
  StaticJsonDocument<200> doc; //括號內為該文檔所分配的RAM
  doc["sensor"] = "DHT11";
  doc["type"] = "temperature";
  doc["value"] = 25;
  
  //To publish a JSON document to an MQTT topic, you need to serialize it to a temporary buffer
  char buffer[256]; //PubSubClient 限制訊息大小為256 bytes(包含標頭)
  size_t n = serializeJson(doc, buffer); //無符號整數定義，據說可以節省CPU週期
  client.publish(outTopic, buffer, n);
  delay(5000); //每五秒發布
  client.endPublish();
  
}
  
