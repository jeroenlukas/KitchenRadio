// (c) Jeroen Lukas 2023

#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include "FS.h"
#include <LittleFS.h>

#include <Ticker.h>

#include <SPI.h>
#include <ezTime.h>
#include <BluetoothA2DPSink.h>

#include "soc/rtc_wdt.h"

#include "configuration/config.h"
#include "configuration/stations.h"
#include "audioplayer/krAudioPlayer.h"
#include "webradio/krWebradio.h"
#include "bluetoothsink/krBluetoothsink.h"
#include "hmi/krFrontPanel.h"
#include "webserver/krWebserver.h"
#include "information/krInfo.h"
#include "information/krWeather.h"
#include "configuration/constants.h"
#include "version.h"

#include "esp_system.h"
#include "esp_himem.h"
#include "esp_heap_caps.h"

#define LED_BUILTIN 2

const char *ssid = CONF_WIFI_SSID;
const char *password = CONF_WIFI_PASSWORD;

int disp_return_time = 0;

Ticker ticker_1s_ref;
Ticker ticker_100ms_ref;
Ticker ticker_5m_ref;
Ticker ticker_1m_ref;

int sound_mode = SOUNDMODE_OFF;

bool f_update_weather = false;
bool f_update_rssi = false;
bool f_update_time = false;
bool f_update_thirdrow = false;

TaskHandle_t xAudioFeedHandle;

// LCD
LiquidCrystal_PCF8574 lcd(0x27);

// Time
Timezone Netherlands;

void lcd_clearrow(int row)
{

    lcd.setCursor(0, row);
    lcd.print("                    ");
    lcd.setCursor(0, row);
}

void ticker_1s()
{
    //int uptime = info_get_int(INFO_UPTIME);
    //info_set_int(INFO_UPTIME, uptime + 1);

    information.system.uptimeSeconds++;

    f_update_rssi = true;
}

void ticker_1m()
{
    f_update_time = true;
    f_update_weather = true;
}

void ticker_100ms()
{
    webserver_handleclient();
    // Read pots
    front_read_pots();
    front_read_buttons();

    if(disp_return_time > 0)
    {
        disp_return_time--;
        if(disp_return_time == 0)
        {
            
            f_update_thirdrow = true;
            
        }
    }
}

void ticker_5m()
{
    //if (weather_retrieve())
    //{
    // Signal a flag, dont write to lcd from this function or it will corrupt the lcd for some reason
    //! f_update_weather = true;
}

void set_sound_mode(int sound_mode_new)
{
    // Do something with the current mode
    switch (sound_mode)
    {
    case SOUNDMODE_OFF:
        // circBuffer.flush();
        break;
    case SOUNDMODE_WEBRADIO:
        if (webradio_isconnected())
            audio_flushbuffer();
        //webradio_client.
        
        break;
    case SOUNDMODE_BLUETOOTH:
        // a2dp_sink.end();
        //  circBuffer.flush();
        // bluetoothsink_end();

        bluetoothsink_end();
        break;
    }

    lcd.setCursor(0, 3);
    lcd_clearrow(3);
    player.softReset();
    delay(100);

    // Switch to the new mode
    switch (sound_mode_new)
    {
    case SOUNDMODE_OFF:
        lcd.printf("%-16s", "(Off)");
        break;
    case SOUNDMODE_WEBRADIO:
        lcd.printf("%-16s", "InternetRadio");
        lcd.setCursor(0, 3);
        lcd.printf("%-16s", stations_name[0]);
        webradio_open_url(stations_host[0], stations_path[0]);
        break;
    case SOUNDMODE_BLUETOOTH:
        lcd.printf("%-16s", "Bluetooth");

        bluetoothsink_start();

        break;
    }

    sound_mode = sound_mode_new;
}

void taskWeatherRetrieve(void *parameter)
{
    while (1)
    {
        vTaskDelay((30 * 1000) / portTICK_PERIOD_MS);
        weather_retrieve();
    }
}

void taskWifiReconnect(void *parameter)
{
    while (1)
    {
        vTaskDelay((3 * 1000) / portTICK_PERIOD_MS);

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Wifi disconnected, attempting to reconnect");
            WiFi.disconnect();
            WiFi.reconnect();
            while (WiFi.status() != WL_CONNECTED)
            {
                Serial.print(".");
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            Serial.println("OK!");
        }
    
    }
}


void setup()
{
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);

    // Info
    //info_set_int(INFO_UPTIME, 0);

    krInfoInitialize();

    front_setup();

    if (BOARD_HAS_PSRAM)
    {
        circBuffer.resize(CIRCBUFFER_SIZE);
    }

    Serial.printf("\nTotal heap: %d", ESP.getHeapSize());
    Serial.printf("\nFree heap: %d", ESP.getFreeHeap());
    Serial.printf("\nTotal PSRAM: %d", ESP.getPsramSize());
    Serial.printf("\nFree PSRAM: %d", ESP.getFreePsram());
    Serial.printf("\nCPU freq: %d", ESP.getCpuFreqMHz());
    Serial.println();
    // SPI
    SPI.begin(18, 19, 23, 32);

    // Inputs
    pinMode(BUTTON_ENCODER, INPUT_PULLUP);
    pinMode(ROTARY_A, INPUT_PULLUP);
    pinMode(ROTARY_B, INPUT_PULLUP);

    // Littlefs
    if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LittleFS Mount Failed");
        return;
    }

    File file = LittleFS.open("/test.txt");
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
    
    Serial.println("File Content:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();

    // LCD
    lcd.begin(20, 4);
    byte deg[] = {B00110, B01001, B01001, B00110, B00000, B00000, B00000, B00000};
    lcd.createChar(LCD_CUST_DEGREE, deg);
    lcd.setBacklight(255);
    lcd.clear();
    lcd.home();
    lcd.printf("KitchenRadio! v%s", kr_version);
    
    lcd.setCursor(0, 2);
    lcd.printf("Connecting to %s", ssid );
    // VS1053
    player.begin();
    player.setVolume(100);

    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname("KitchenRadio");
    WiFi.disconnect();

    WiFi.begin(ssid, password);
    Serial.printf("Connecting to WiFi %s..", ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 0);
    lcd.printf("IP: %s", WiFi.localIP().toString().c_str());

    waitForSync();

    Netherlands.setLocation("Europe/Amsterdam");
    f_update_time = true;

    player.setVolume(70);

    webserver_setup();

    // Tickers
    ticker_1s_ref.attach(1, ticker_1s);
    ticker_100ms_ref.attach(0.1, ticker_100ms);
    ticker_5m_ref.attach(300, ticker_5m);
    ticker_1m_ref.attach(60, ticker_1m);

    delay(100);

    bluetoothsink_setup();

    delay(100);

    set_sound_mode(SOUNDMODE_OFF);

    xTaskCreate(taskWeatherRetrieve, "wear", 10000, NULL, 1, NULL);
    xTaskCreate(taskWifiReconnect, "wifiReconnect", 10000, NULL, 2, NULL);

    player.setBass(7);
    player.setTreble(-2);

    lcd.clear();
}

void loop()
{
    static int station = 0;

    if (sound_mode == SOUNDMODE_WEBRADIO)
        webradio_handle_stream();
    //    else if (sound_mode == SOUNDMODE_BLUETOOTH)
    //  bluetoothsink_handle_stream();
    audio_feedbuffer(sound_mode);

    front_read_encoder();

    front_multibuttons_loop();

    // ------=== FLAGS ===-------
    if (f_front_pot_volume_changed)
    {
        f_front_pot_volume_changed = false;
        Serial.println(front_pot_vol);
        lcd.setCursor(0, 2);
        lcd.printf("Volume: %3d", (int)map(front_pot_vol, 0, 4095, 0, 100));
        player.setVolume(log(front_pot_vol + 1) / log(4095) * 100);

        disp_return_time = DISPLAY_RETURN_TIME;
    }

  /*  if (f_front_pot_treble_changed)
    {
        f_front_pot_treble_changed = false;

        int8_t nibble_tone = map(front_pot_treble, 0, 4095, -8, 7);

        player.setTreble(nibble_tone);

        lcd.setCursor(0, 2);
        lcd.printf("Treble: %3d", nibble_tone);
    }

    if (f_front_pot_bass_changed)
    {
        f_front_pot_bass_changed = false;

        int8_t nibble_bass = map(front_pot_bass, 0, 4095, 0, 15);

        player.setBass(nibble_bass);

        lcd.setCursor(0, 2);
        lcd.printf("Bass:   %3d", (nibble_bass - 8));
    }*/

    if (f_front_button_encoder_pressed)
    {
        f_front_button_encoder_pressed = false;

        if(sound_mode == SOUNDMODE_BLUETOOTH)
        {
            bluetoothsink_playpause();
        }
        // Toggle sound mode
        /*if (sound_mode < 2)
            set_sound_mode(sound_mode + 1);
        else
            set_sound_mode(0);*/
    }

    if (f_front_encoder_turn_left)
    {
        f_front_encoder_turn_left = false;
        Serial.println("left");
        if (sound_mode == SOUNDMODE_WEBRADIO && station > 0)
        {
            station--;
            webradio_open_url(stations_host[station], stations_path[station]);
            lcd.setCursor(0, 3);
            lcd.printf("%-16s", stations_name[station]);
            audio_getVS1053info();
        }

        else if (sound_mode == SOUNDMODE_BLUETOOTH)
        {
            bluetoothsink_previous();
        }
    }

    if (f_front_encoder_turn_right)
    {
        f_front_encoder_turn_right = false;
        Serial.println("right");

        if (sound_mode == SOUNDMODE_WEBRADIO && station < (stations_cnt - 1))
        {
            station++;
            webradio_open_url(stations_host[station], stations_path[station]);
            lcd.setCursor(0, 3);
            lcd.printf("%-16s", stations_name[station]);
            audio_getVS1053info();
        }

        if (sound_mode == SOUNDMODE_BLUETOOTH)
        {
            bluetoothsink_next();
        }
    }

    audio_feedbuffer(sound_mode);

    if (f_bluetoothsink_metadata_received)
    {
        f_bluetoothsink_metadata_received = false;
        lcd.setCursor(0, 3);
        lcd.printf("BT: %.16s", bluetooth_media_title);
        audio_getVS1053info();
    }

    audio_feedbuffer(sound_mode);

    if (f_update_weather)
    {
        f_update_weather = false;
        
    }

    if(f_update_thirdrow)
    {
        f_update_thirdrow = false;
        lcd_clearrow(2);
    }



    if (f_update_rssi)
    {
        f_update_rssi = false;
        lcd.setCursor(0, 1);
        information.system.wifiRSSI = WiFi.RSSI();
        //lcd.printf("RSSI:%ddBm %7d", WiFi.RSSI(), circBuffer.available());
    }

    audio_feedbuffer(sound_mode);

    if (f_update_time)
    {
        f_update_time = false;
        lcd_clearrow(0);
        lcd.setCursor(0,0);
        lcd.printf("%.1f", information.weather.temperature);
        lcd.write(byte(LCD_CUST_DEGREE));
        lcd.write("C");
        lcd.setCursor(15, 0);
        lcd.printf(Netherlands.dateTime("H:i").c_str());

        lcd_clearrow(1);
        lcd.setCursor(0,1);
        lcd.printf("%.0fkm/h", round(information.weather.windSpeedKmh));
        lcd.setCursor(11, 1);
        lcd.printf(Netherlands.dateTime("D d M").c_str());
        //info_set_string(INFO_TIME_SHORT, Netherlands.dateTime("H:i"));
        information.timeShort = Netherlands.dateTime("H:i");
    }

    if(f_button_off_pressed)
    {
        f_button_off_pressed = false;
        set_sound_mode(SOUNDMODE_OFF);
    }
    if(f_button_radio_pressed)
    {
        f_button_radio_pressed = false;
        set_sound_mode(SOUNDMODE_WEBRADIO);
    }
    if(f_button_bluetooth_pressed)
    {
        f_button_bluetooth_pressed = false;
        set_sound_mode(SOUNDMODE_BLUETOOTH);
    }
}
