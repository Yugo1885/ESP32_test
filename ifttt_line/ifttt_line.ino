/*原作者:https://atceiling.blogspot.com/2019/07/arduino38dht-11-line.html*/
#include <WiFi.h>
#include <DHT.h>
#define DHTTYPE DHT11
#define DHTPIN 2

char ssid[] = "your_ssid";
char password[] ="your_password";

DHT dht(DHTPIN, DHTTYPE);
float temp;
float hum;
String TEMP = "";
String HUM = "";

unsigned long previousMillis = 0;
const long initerval = 2000;
const char* host = "maker.ifttt.com";
const int httpPort = 80;

void setup(){
  Serial.begin(9600);
  dht.begin();
  //等待連線
  Serial.println("Connecting to Wifi");
  while(WiFi.status() !=WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    if(isnan(temp) || isnan(hum)){
      Serial.println("Falied to read from DHT sensor!");
      return;
    }
  }
  TEMP = "Temperature:"+String((int)temperature)+"oC";
  HUM = "Humidity:"+String((int)humidity)+"%";
  Serial.println(TEMP);
  Serial.println(HUM);
  
  WiFiClient client;
  Serial.print("connecting to ");
  Serial.println(host);
  if(!client.connect(host,httpPort)){
    Serial.println("connection failed");
    return;
  }
  //xxx-xxxx-xxx為自身金鑰
  String getStr_line = "GET /trigger/Line_msg/with/key/XXX-XXXX-XXX?value1="+TEMP+"&value2="+HUM+" HTTP/1.1\r\n"
  + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP32\r\n" + "Connection: close\r\n\r\n";
  
  Serial.println(getStr_line);
  client.print(getStr_line);
  client.stop();
  
  delay(60000);
}
