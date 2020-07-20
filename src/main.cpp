#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "RTClib.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#define NUM_SPRINKLERS 8
RTC_DS3231 rtc;

const char* ssid = "ESP32-AP";

AsyncWebServer server(80);

struct Sprinkler {
  bool active;
  char start[6];
  int duration;
  bool days[7];
} sprinklers[] = {
  { false, "00:00", 2, { true, false, false, false, false, false, false } },
  { false, "01:00", 4, { false, true, false, false, false, false, false } },
  { false, "02:00", 6, { false, false, true, false, false, false, false } },
  { false, "03:00", 8, { false, false, false, true, false, false, false } },
  { false, "04:00", 10, { false, false, false, false, true, false, false } },
  { false, "05:00", 12, { false, false, false, false, false, true, false } },
  { false, "06:00", 14, { false, false, false, false, false, false, true } },
  { false, "07:00", 16, { true, false, false, false, false, false, false } }
};


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

void getTime(AsyncWebServerRequest *request)
{
    Serial.println("GET /time");
    DateTime now = rtc.now();
    char buffer[] = "DDD, DD MMM YYYY hh:mm:ss";
    String date = now.toString(buffer);
    Serial.println(date);
    request->send(200, "text/plain", date);
}

void postTime(AsyncWebServerRequest *request)
{
  Serial.println("POST /time");
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
}

void serializeSprinkler(JsonObject &root, int id) {
  root["id"] = id;
  root["start"] = sprinklers[id].start;
  root["duration"] = sprinklers[id].duration;
  JsonArray& days = root.createNestedArray("days");
  for (int i = 0; i <= 7; i++) {
    JsonObject& day = days.createNestedObject();
    char j[2];
    sprintf(j, "%d", i);
    day[j] = sprinklers[id].days[i];
  }
}

void getSprinkler(AsyncWebServerRequest *request)
{
  Serial.println("GET /sprinkler");
  if(request->hasParam("id")) {
    AsyncWebParameter* p = request->getParam("id");
    int id = atoi(p->value().c_str());
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    serializeSprinkler(root, id);
    root.printTo(*response);
    request->send(response);    
  } else {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonArray &root = jsonBuffer.createArray();
    for (int i = 0; i < NUM_SPRINKLERS; i++) {
      JsonObject &sprinkler = root.createNestedObject();
      serializeSprinkler(sprinkler, i);
    }
    root.printTo(*response);
    request->send(response);    
  }

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
  Serial.print("IP address: ");
  Serial.println(IP);

  SPIFFS.begin();

  server.on("/time", HTTP_GET, getTime); 
  server.on("/time", HTTP_POST, postTime); 
  server.on("/sprinkler", HTTP_GET, getSprinkler);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(notFound);
  server.begin();
}

void loop()
{
}
