#include <Arduino.h>
#include <WebServer.h>
#include "FS.h"
#include <LittleFS.h>

#include "hmi/krFrontPanel.h"
#include "webserver/krWebserver.h"
#include "time.h"
#include "information/krInfo.h"



WebServer webserver(80);
String webheader = "<html><head><title>KitchenRadio</title></head><body bgcolor='#cccccc'><font face='Arial'><h1>&#128251; KitchenRadio</h1><hr>";

void handleRootPath()
{ //Handler for the rooth path

    String content;

    File file = LittleFS.open("/test.txt");
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
    String fileContent;
    
    while(file.available()){
        fileContent += char(file.read());        
    }
    file.close();

    content = webheader + 
            "<br />Current time: " + information.timeShort +
            "<br />System uptime: " + String(information.system.uptimeSeconds) + " seconds"
            "<br />WiFI RSSI: " + String(information.system.wifiRSSI) + " dBm"
            "<h3>&#127926; Audio</h3>"
            "<table>"
            "<tr><td width='200'>Sample rate:</td><td>" + String((uint16_t)information.audioPlayer.sampleRate) + " Hz</td></tr>"
            "<tr><td>Channels:</td><td>" + String(information.audioPlayer.channels) + "</td></tr>"
            "<tr><td>Bitrate:</td><td>" + String(information.audioPlayer.bitRate) + " kbps</td></tr>"
            "</table>"
            "<h3>&#9925; Weather</h3>"
            "<table>"
            "<tr><td width='200'>Temperature:</td><td>" + String(information.weather.temperature) + " &deg;C</td></tr>"
            "<tr><td>Windspeed:</td><td>" + String(information.weather.windSpeedKmh) + " km/h</td></tr>"
            "</table>"
            "<hr><i>"
            + (fileContent) + 
            "</i><hr>"
            "<a href='/mode_off'>Off</a> <a href='/mode_radio'>Radio</a> <a href='/mode_bluetooth'>Bluetooth</a><br />"
            "<a href='/prev'>&larr;</a> <a href='/next'>&rarr;</a>"
            "<form method='GET' action='/set_volume'><input type='range' name='volume' min='0' max='100'><input type='submit' value='Set'></form>"
            ;

    webserver.send(200, "text/html", content);
}

void handle_mode_off()
{
    f_button_off_pressed = true;
    handleRootPath();
}

void handle_mode_radio()
{
    f_button_radio_pressed = true;

    handleRootPath();
}

void handle_mode_bluetooth()
{
    f_button_bluetooth_pressed = true;

    handleRootPath();
}

void handle_prev()
{
    f_front_encoder_turn_left = true;

    handleRootPath();
}

void handle_next()
{
    f_front_encoder_turn_right = true;

    handleRootPath();
}

void handle_setvolume()
{
    f_front_encoder_turn_right = true;

    handleRootPath();
}

void webserver_setup()
{
    webserver.on("/", handleRootPath); //Associate the handler function to the path
    webserver.on("/mode_off", handle_mode_off);
    webserver.on("/mode_radio", handle_mode_radio);
    webserver.on("/mode_bluetooth", handle_mode_bluetooth);
    webserver.on("/prev", handle_prev);
    webserver.on("/next", handle_next);
    webserver.on("/set_volume", handle_setvolume);

    webserver.begin();
}

void webserver_handleclient()
{
    webserver.handleClient();
}