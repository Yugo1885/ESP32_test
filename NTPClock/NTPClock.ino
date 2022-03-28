#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "SUGAR S11";
const char* password = "0910388155";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;

void setup() {
  Serial.begin(115200);
  connectWiFi();
  timeClient.begin();
  timeClient.setTimeOffset(28800); //your timezone
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("WiFi");
  display.setCursor(0,25);
  display.println("connected");
  display.display();
  delay(2000);
}

void loop() {
  getLocalTime();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,15);
  display.print(formattedDate);
  display.display();
}

void connectWiFi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}

void getLocalTime(){
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);
  delay(1000);
}
