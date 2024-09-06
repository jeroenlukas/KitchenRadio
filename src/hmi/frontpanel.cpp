#include <Arduino.h>
#include <RotaryEncoder.h>
#include <BfButtonManager.h>
#include <BfButton.h>
#include "configuration/configMisc.h"
#include "configuration/configPinout.h"
#include "hmi/frontpanel.h"

// To calibrate button ADC values
//#define CAL_BUTTONS

int adc_pot_vol = 0;
int adc_pot_treble = 0;
int adc_pot_bass = 0;

int prev_adc_pot_vol = 0;
int prev_adc_pot_treble = 0;
int prev_adc_pot_bass = 0;

// Button states
bool prev_button_encoder = true;

// Externally available
bool f_front_pot_volume_changed = false;
bool f_front_pot_treble_changed = false;
bool f_front_pot_bass_changed = false;

bool f_front_button_encoder_pressed = false;

bool f_front_encoder_turn_left = false;
bool f_front_encoder_turn_right = false;

int front_pot_vol = 0;
int front_pot_treble = 0;
int front_pot_bass = 0;

bool f_button_off_pressed = false;
bool f_button_radio_pressed = false;
bool f_button_bluetooth_pressed = false;

uint16_t reading, avg;
uint16_t sum = 0;

const unsigned int pin = BUTTONS;

BfButtonManager buttonmanager(BUTTONS, 6);
BfButton btn_off(BfButton::ANALOG_BUTTON_ARRAY, 0);
BfButton btn_radio(BfButton::ANALOG_BUTTON_ARRAY, 1);
BfButton btn_bluetooth(BfButton::ANALOG_BUTTON_ARRAY, 2);
BfButton btn_system(BfButton::ANALOG_BUTTON_ARRAY, 3);
BfButton btn_alarm(BfButton::ANALOG_BUTTON_ARRAY, 4);
BfButton btn_lamp(BfButton::ANALOG_BUTTON_ARRAY, 5);

// Rotary encoder
RotaryEncoder encoder(ROTARY_A, ROTARY_B, RotaryEncoder::LatchMode::FOUR3);

void button_press_handler(BfButton *btn, BfButton::press_pattern_t pattern)
{
   switch(btn->getID())
   {
    case 0:
        f_button_off_pressed = true;
        break;
    case 1:
        f_button_radio_pressed = true;
        break;
    case 2:
        f_button_bluetooth_pressed = true;
        break;
    default:
        break;
   }

   /*  Serial.print(btn->getID());
  switch (pattern)  {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      break;
  }*/
}

void front_setup()
{
    btn_off.onPress(button_press_handler);
    btn_off.onDoublePress(button_press_handler);
    btn_off.onPressFor(button_press_handler, 1000);
    buttonmanager.addButton(&btn_off, BTN_ADC_OFF_MIN, BTN_ADC_OFF_MAX);

    btn_radio.onPress(button_press_handler);
    btn_radio.onDoublePress(button_press_handler);
    btn_radio.onPressFor(button_press_handler, 1000);
    buttonmanager.addButton(&btn_radio, BTN_ADC_RADIO_MIN, BTN_ADC_RADIO_MAX);

    btn_bluetooth.onPress(button_press_handler);
    btn_bluetooth.onDoublePress(button_press_handler);
    btn_bluetooth.onPressFor(button_press_handler, 1000);
    buttonmanager.addButton(&btn_bluetooth, BTN_ADC_BLUETOOTH_MIN, BTN_ADC_BLUETOOTH_MAX);

    btn_system.onPress(button_press_handler);
    btn_system.onDoublePress(button_press_handler);
    btn_system.onPressFor(button_press_handler, 1000);
    buttonmanager.addButton(&btn_system, BTN_ADC_SYSTEM_MIN, BTN_ADC_SYSTEM_MAX);

    btn_alarm.onPress(button_press_handler);
    btn_alarm.onDoublePress(button_press_handler);
    btn_alarm.onPressFor(button_press_handler, 1000);
    buttonmanager.addButton(&btn_alarm, BTN_ADC_ALARM_MIN,BTN_ADC_ALARM_MAX);

    btn_lamp.onPress(button_press_handler);
    btn_lamp.onDoublePress(button_press_handler);
    btn_lamp.onPressFor(button_press_handler, 1000);
    buttonmanager.addButton(&btn_lamp,BTN_ADC_LAMP_MIN,BTN_ADC_LAMP_MAX); 
    
    

    buttonmanager.begin();
}



void front_read_pots()
{
    adc_pot_vol = analogRead(POT_VOLUME);

    if (adc_pot_vol < (prev_adc_pot_vol - POT_HYST) || adc_pot_vol > (prev_adc_pot_vol + POT_HYST))
    {
        f_front_pot_volume_changed = true;
        front_pot_vol = 4095 - adc_pot_vol;
    }

    prev_adc_pot_vol = adc_pot_vol;
    prev_adc_pot_treble = adc_pot_treble;
    prev_adc_pot_bass = adc_pot_bass;
}

void front_multibuttons_loop()
{
    buttonmanager.loop();
}

void front_read_buttons()
{
    // Encoder switch
    if (digitalRead(BUTTON_ENCODER) < prev_button_encoder)
    {
        f_front_button_encoder_pressed = true;
    }

    // Multibuttons
    #ifdef CAL_BUTTONS
    static unsigned int i = 0;
    reading = BfButtonManager::printReading(pin);
    if (reading > 100) { // button pressed
        sum += reading;
        if (i == 4) {
        avg = sum / 5;
        Serial.print("Avarage Reading: ");
        Serial.println(avg);
        sum = 0;
        }
        i++;
        if (i > 4) i = 0;
    } else { // button released
        sum = 0;
        i = 0;
    }
    #endif

    prev_button_encoder = digitalRead(BUTTON_ENCODER);
}

void front_read_encoder()
{
    static int pos = 0;
    encoder.tick();

    int newPos = encoder.getPosition();
    if (pos != newPos)
    {
        if ((int)(encoder.getDirection()) == 1)
        {
            f_front_encoder_turn_right = true;
        }
        else
        {
            f_front_encoder_turn_left = true;
        }
        pos = newPos;
    }
}