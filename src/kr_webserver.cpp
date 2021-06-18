#include <Arduino.h>
#include <WebServer.h>

#include "kr_webserver.h"
#include "time.h"
#include "kr_info.h"

WebServer webserver(80);
String webheader = "<html><head><title>KitchenRadio</title></head><body><h1>&#128251; KitchenRadio</h1><hr>";

void handleRootPath()
{ //Handler for the rooth path

    String content;

    content = webheader + 
            "<br />Current time: " + 
            "<br />System uptime: " + String(info_get_int(INFO_UPTIME)) + " seconds"
            "<br />Sample rate: " + String((uint16_t)info_get_int(INFO_AUDIO_SAMPLERATE)) + " Hz"
            "<br />Channels: " + String(info_get_int(INFO_AUDIO_CHANNELS)) +
            "<br />Bitrate: " + String(info_get_int(INFO_AUDIO_BITRATE)) + " bps"; 
    webserver.send(200, "text/html", content);
}

void webserver_setup()
{
    webserver.on("/", handleRootPath); //Associate the handler function to the path
    webserver.begin();
}

void webserver_handleclient()
{
    webserver.handleClient();
}