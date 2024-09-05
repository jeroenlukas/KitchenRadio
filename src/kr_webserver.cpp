#include <Arduino.h>
#include <WebServer.h>

#include "frontpanel.h"
#include "kr_webserver.h"
#include "time.h"
#include "kr_info.h"

WebServer webserver(80);
String webheader = "<html><head><title>KitchenRadio</title></head><body><font face='Arial'><h1>&#128251; KitchenRadio</h1><hr>";

void handleRootPath()
{ //Handler for the rooth path

    String content;

    content = webheader + 
            "<br />Current time: " + info_get_string(INFO_TIME_SHORT) +
            "<br />System uptime: " + String(info_get_int(INFO_UPTIME)) + " seconds"
            "<h3>&#127926; Audio</h3>"
            "<table>"
            "<tr><td width='200'>Sample rate:</td><td>" + String((uint16_t)info_get_int(INFO_AUDIO_SAMPLERATE)) + " Hz</td></tr>"
            "<tr><td>Channels:</td><td>" + String(info_get_int(INFO_AUDIO_CHANNELS)) + "</td></tr>"
            "<tr><td>Bitrate:</td><td>" + String(info_get_int(INFO_AUDIO_BITRATE)) + " kbps</td></tr>"
            "</table>"
            "<h3>&#9925; Weather</h3>"
            "<table>"
            "<tr><td width='200'>Temperature:</td><td>" + String(info_get_float(INFO_WEATHER_TEMP)) + " &deg;C</td></tr>"
            "<tr><td>Windspeed:</td><td>" + String(info_get_float(INFO_WEATHER_WIND_KMH)) + " km/h</td></tr>"
            "</table>"
            "<hr>"
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