#include <Arduino.h>
#include <WebServer.h>

#include "kr_webserver.h"
#include "time.h"

WebServer webserver(80);
String webheader = "<html><head><title>KitchenRadio</title></head><body><h1>&#128251; KitchenRadio</h1><hr>";

void handleRootPath()
{ //Handler for the rooth path

    String content;

    content = webheader + "<br />Current time: " + "<br />System uptime: " + " seconds"; // + timeClient.getFormattedTime() + "<br />System uptime: " + String(uptime) + " sec";
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