#include <Arduino.h>
//#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "information/krInfo.h"
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>

AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
//JSONVar readings;
 //DynamicJsonDocument readings(1024);

// Get Sensor Readings and return JSON object
String getSensorReadings(){
    //readings = information.system.wifiRSSI;
    String jsonString;
    DynamicJsonDocument doc(1024);
    doc["uptimeSeconds"]=information.system.uptimeSeconds;
    doc["rssi"]=information.system.wifiRSSI;
    serializeJson(doc,jsonString);
   // serializeJson(String(information.system.uptimeSeconds),readings);
    //String jsonString = JSON.stringify(readings);
    return jsonString;
}

void notifyClients(void) {
    String sensorReadings = getSensorReadings();
    ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;
    //String message = (char*)data;
    // Check if the message is "getReadings"
    //if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      //String sensorReadings = getSensorReadings();
      //Serial.print(sensorReadings);
      notifyClients();
    //}
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
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/index.html", "text/html");
  });

    server.serveStatic("/", LittleFS, "/www/");

  // Start server
  server.begin();
}
