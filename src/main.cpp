#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "RTClib.h"

RTC_DS3231 rtc;

const char* ssid = "ESP32-AP";

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request)
{
  Serial.printf("NOT_FOUND: ");
  if(request->method() == HTTP_GET)
    Serial.printf("GET");
  else if(request->method() == HTTP_POST)
    Serial.printf("POST");
  else if(request->method() == HTTP_DELETE)
    Serial.printf("DELETE");
  else if(request->method() == HTTP_PUT)
    Serial.printf("PUT");
  else if(request->method() == HTTP_PATCH)
    Serial.printf("PATCH");
  else if(request->method() == HTTP_HEAD)
    Serial.printf("HEAD");
  else if(request->method() == HTTP_OPTIONS)
    Serial.printf("OPTIONS");
  else
    Serial.printf("UNKNOWN");
  Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

  if(request->contentLength()){
    Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
    Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
  }

  int headers = request->headers();
  int i;
  for(i=0;i<headers;i++){
    AsyncWebHeader* h = request->getHeader(i);
    Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }

  int params = request->params();
  for(i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){
      Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }

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

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   Serial.println("/ GET");
  //   request->send(SPIFFS, "/index.html");
  // }); 
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound(notFound);
  server.begin();
}

void loop()
{
}
