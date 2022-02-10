#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_ssid"; 
const char* password = "your_password"; 

const char* mqtt_server = "your_mqtt_broker";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 4;
#define subTopic "esp32/led"

void setup(){
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}
//WiFi連線設定
void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//回呼函數
void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(".Message: ");
  String messageTemp;

  for (int i=0; i<length; i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(String(topic) == subTopic){
    Serial.print("Changing led to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}
//假如斷線需重新連線
void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    if(client.connect("ESP32Client")){
      Serial.println("connected");
      client.subscribe(subTopic);
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  
}
