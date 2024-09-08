#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include "information/krInfo.h"
#include "configuration/config.h"

//#include "configMisc.h"

HTTPClient http;

const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=Zierikzee,nl&units=metric&APPID=";
const String key = CONFIG_OPENWEATHER_KEY;

float weather_temperature = 0.0;
float windspeed_kmh = 0.0;
float windspeed = 0.0;

int weather_temperature_int = 0;

bool weather_retrieve()
{
    bool ret = false;
    http.begin(endpoint + key);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);

        deserializeJson(doc, payload);

        information.weather.temperature = doc["main"]["temp"];        
        information.weather.windSpeedKmh = (double)(doc["wind"]["speed"]) * 3.6;
        
        ret = true;
    }
    else
        Serial.println("weather_retrieve: Error on HTTP request");

    http.end();
    
    return ret;
}