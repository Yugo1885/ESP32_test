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
String TEMP;
String HUM;

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
    
}
