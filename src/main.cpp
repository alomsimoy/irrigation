#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "RTClib.h"

RTC_DS3231 rtc;

const char* ssid = "ESP32-AP";

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404);
}

void setup()
{
  Serial.begin(115200);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while(1);
  }

  Serial.println("Setting up Access Point...");

  WiFi.softAP(ssid);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  SPIFFS.begin();

  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/time GET");
    DateTime now = rtc.now();
    char buffer[] = "DDD, DD MMM YYYY hh:mm:ss";
    String date = now.toString(buffer);
    Serial.println(date);
    request->send(200, "text/plain", date);
  }); 

  server.on("/time", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("/time POST");
    if(request->hasParam("time", true)) {
      AsyncWebParameter* p = request->getParam("time", true);
      int timezone = 0; // timezone offset in minutes
      if (request->hasParam("timezone", true)) {
        AsyncWebParameter* tz = request->getParam("timezone", true);
        timezone = atoi(tz->value().c_str());
      }
      Serial.print("Setting time to ");
      Serial.print(p->value());
      Serial.print(" with a TZ offset of ");
      Serial.println(timezone);
      uint32_t t = atoi(p->value().c_str()) - timezone * 60;
      rtc.adjust(DateTime(t));
    }
    request->send(200, "text/plain", "Done");
  }); 

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/ GET");
    request->send(SPIFFS, "/index.html");
  }); 

  server.onNotFound(notFound);
  server.begin();
}

void loop()
{
}
