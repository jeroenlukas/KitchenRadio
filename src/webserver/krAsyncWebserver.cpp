#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "information/krInfo.h"
#include "version.h"
#include "flags.h"

// https://randomnerdtutorials.com/esp32-websocket-server-sensor/

AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

void notifyClients(void) {
    String jsonString;
    DynamicJsonDocument doc(1024);

    char rounded[10] = "";

    doc["kr-version"] = kr_version;

    doc["system-uptimeSeconds"]=information.system.uptimeSeconds;
    doc["system-rssi"]=information.system.wifiRSSI;

    sprintf(rounded, "%.1f", information.weather.temperature);
    doc["weather-temperature"] = rounded;
    sprintf(rounded, "%.1f", information.weather.windSpeedKmh);
    doc["weather-windspeedkmh"] = rounded;
    //doc["weather-windspeedkmh"] = information.weather.windSpeedKmh;

    doc["audioplayer-volume"] = information.audioPlayer.volume;
    serializeJson(doc,jsonString);
    
    ws.textAll(jsonString);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      
      data[len] = '\0';

      String message = String((char*)data);
      Serial.printf("received: %s\n", message.c_str());
      
      if(message == "buttonOffPressed")
      {
        flags.frontPanel.buttonOffPressed = true;
      }
      else if(message == "buttonWebradioPressed")
      {
        flags.frontPanel.buttonRadioPressed = true;
      }
      
  }
}

void awsCleanupClients(void)
{
    ws.cleanupClients();
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  Serial.print("Starting websocket.");
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/index.html", "text/html");
  });

    server.serveStatic("/", LittleFS, "/www/");

  // Start server
  server.begin();
}
