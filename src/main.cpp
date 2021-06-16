#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <Ticker.h>
#include <SPI.h>

#include <BluetoothA2DPSink.h>

#include "configMisc.h"
#include "main.h"
#include "VS1053.h"
#include "WebRadio.h"
#include "bluetoothSink.h"
#include "frontpanel.h"
#include "helloMp3.h"

#define LED_BUILTIN 2

const char *ssid = CONF_WIFI_SSID;
const char *password = CONF_WIFI_PASSWORD;

Ticker ticker_1s_ref;

#define SOUNDMODE_OFF 0
#define SOUNDMODE_WEBRADIO 1
#define SOUNDMODE_BLUETOOTH 2

int sound_mode = SOUNDMODE_OFF;

// LCD
LiquidCrystal_PCF8574 lcd(0x27);

bool first = true;

void ticker_1s()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void set_sound_mode(int sound_mode_new)
{
  Serial.printf("Current: %d - new: %d", sound_mode, sound_mode_new);

  // Do something with the current mode
  switch (sound_mode)
  {
  case SOUNDMODE_OFF:
    // circBuffer.flush();
    break;
  case SOUNDMODE_WEBRADIO:
    //if (webradio_client.connected() > 0)
    //webradio_client.
    // circBuffer.flush();
    break;
  case SOUNDMODE_BLUETOOTH:
    // a2dp_sink.end();
    //  circBuffer.flush();
    a2dp_sink.end(false);

    //a2dp_sink.r
    break;
  }

  sound_mode = sound_mode_new;

  lcd.setCursor(0, 3);
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
    webradio_open_url(0);
    break;
  case SOUNDMODE_BLUETOOTH:
    lcd.printf("%-16s", "Bluetooth");

    a2dp_sink.start("KitchenRadio");
    circBuffer.flush();
    circBuffer.write((char *)bt_wav_header, 44); // very important!!!
    break;
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  // Tickers
  ticker_1s_ref.attach(1, ticker_1s);

  // SPI
  SPI.begin(18, 19, 23, 32);

  // Analog

  // Inputs
  pinMode(BUTTON_ENCODER, INPUT_PULLUP);

  // Encoder

  player.begin();
  player.setVolume(100);

  lcd.begin(20, 4);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.home();
  lcd.print("Hallo");

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname("KitchenRadio");
  WiFi.disconnect();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0);
  lcd.printf("IP: %s", WiFi.localIP().toString().c_str());

  player.setVolume(70);
  delay(100);
  player.playChunk(hello2, sizeof(hello2)); //VS1053 is wake up & running

  delay(100);

  //webradio_open_url(0);

  // a2dp_sink.start("KitchenRadio");
  a2dp_sink.set_stream_reader(bluetoothsink_read_data_stream, false);
  a2dp_sink.set_avrc_metadata_callback(bluetoothsink_avrc_metadata_callback);
  //circBuffer.write((char *)bt_wav_header, 44); // very important!!!
  //player.playChunk(bt_wav_header, 44);
  set_sound_mode(SOUNDMODE_OFF);
}

void loop()
{
  static int lastms_1s = 0;
  static int lastms_100ms = 0;

  static int station = 0;

  if (sound_mode == SOUNDMODE_WEBRADIO)
    webradio_handle_stream();
  else if (sound_mode == SOUNDMODE_BLUETOOTH)
    bluetoothsink_handle_stream();

  if (millis() - lastms_1s > 1000)
  {
    // Execute this once every second - for lower priority tasks
    lastms_1s = millis();
    lcd.setCursor(0, 1);
    lcd.printf("RSSI: %d dBm %6d", WiFi.RSSI(), circBuffer.available());
    lcd.setCursor(0, 3);
    //lcd.printf("BR: %ld", a2dp_sink.sample_rate);
    //player.printDetails("fadsa");
  }

  if (millis() - lastms_100ms > 100)
  {
    // Execute this once 100 ms - for lower priority tasks
    lastms_100ms = millis();

    // Read pots
    front_read_pots();
    front_read_buttons();
  }

  // ------=== FLAGS ===-------
  if (f_front_pot_volume_changed)
  {
    f_front_pot_volume_changed = false;

    lcd.setCursor(0, 2);
    lcd.printf("Volume: %3d", (int)map(front_pot_vol, 0, 4095, 0, 100));
    player.setVolume(log(front_pot_vol + 1) / log(4095) * 100);
  }

  if (f_front_pot_treble_changed)
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
  }

  if (f_front_button_encoder_pressed)
  {
    f_front_button_encoder_pressed = false;

    Serial.println("Button pressed.");
    // Toggle sound mode
    if (sound_mode < 2)
      set_sound_mode(sound_mode + 1);
    else
      set_sound_mode(0);
  }

  if (f_front_encoder_turn_left)
  {
    f_front_encoder_turn_left = false;

    if (sound_mode == SOUNDMODE_WEBRADIO && station > 0)
    {
      station--;
      webradio_open_url(station);
      lcd.setCursor(0, 3);
      lcd.printf("%-16s", stations_name[station]);
      Serial.println("prev");
    }

    else if (sound_mode == SOUNDMODE_BLUETOOTH)
    {
      a2dp_sink.previous();
    }
  }

  if (f_front_encoder_turn_right)
  {
    f_front_encoder_turn_right = false;

    if (sound_mode == SOUNDMODE_WEBRADIO && station < (stations_cnt - 1))
    {
      station++;
      webradio_open_url(station);
      lcd.setCursor(0, 3);
      lcd.printf("%-16s", stations_name[station]);
      Serial.println("next");
    }

    if (sound_mode == SOUNDMODE_BLUETOOTH)
    {
      a2dp_sink.next();
    }
  }

  if (f_bluetoothsink_metadata_received)
  {
    f_bluetoothsink_metadata_received = false;
    lcd.setCursor(0, 3);
    lcd.printf("BT: %.16s", bluetooth_media_title);
  }
}
