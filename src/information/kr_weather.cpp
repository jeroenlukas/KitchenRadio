#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include "information/kr_info.h"
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
       // Serial.println(httpCode);
        //Serial.println(payload);
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        weather_temperature = doc["main"]["temp"];
        windspeed = doc["wind"]["speed"];
        
        weather_temperature_int = round(weather_temperature);
        windspeed_kmh = windspeed * 3.6;
        //temperature = temperature_str.toDouble();
        //Serial.printf("temp: %f - wind: %f km/h", weather_temperature, windspeed_kmh );

        info_set_float(INFO_WEATHER_TEMP, weather_temperature);
        info_set_float(INFO_WEATHER_WIND_KMH, windspeed_kmh);

        ret = true;
    }
    else
        Serial.println("weather_retrieve: Error on HTTP request");

    http.end();
    
    return ret;
}