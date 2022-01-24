#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h> //非同步網站伺服器程式庫

const char* ssid = "your_ssid";
const char* password = "your_password";

AsyncWebServer server(80);

Adafruit_BMP085 bmp;
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  WiFi.mode(WIFI_STA); //設置成WiFi終端
  WiFi.begin(ssid, password);
  Serial.println("");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/sensor.json", HTTP_GET, [](AsyncWebServerRequest *req){
    StaticJsonDocument<48> doc;

    doc["temp"] = bmp.readTemperature();
    doc["alti"] = bmp.readAltitude();
    doc["pres"] = bmp.readPressure();

    String resp;
    serializeJson(doc, resp); //Json轉字串
    req->send(200, "application/json", resp); //送出Json文件的回應
    
  });
  server.begin();

}

void loop() {
}
