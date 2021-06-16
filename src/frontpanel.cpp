#include <Arduino.h>
#include <RotaryEncoder.h>

#include "configMisc.h"
#include "configPinout.h"
#include "frontpanel.h"

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

// Rotary encoder
RotaryEncoder encoder(ROTARY_A, ROTARY_B, RotaryEncoder::LatchMode::FOUR3);

void front_read_pots()
{
    adc_pot_vol = analogRead(POT_VOLUME);
    adc_pot_treble = analogRead(POT_TREBLE);
    adc_pot_bass = analogRead(POT_BASS);

    if (adc_pot_vol < (prev_adc_pot_vol - POT_HYST) || adc_pot_vol > (prev_adc_pot_vol + POT_HYST))
    {
        f_front_pot_volume_changed = true;
        front_pot_vol = adc_pot_vol;
    }
    if (adc_pot_treble < (prev_adc_pot_treble - POT_HYST) || adc_pot_treble > (prev_adc_pot_treble + POT_HYST))
    {
        f_front_pot_treble_changed = true;
        front_pot_treble = adc_pot_treble;
    }
    if (adc_pot_bass < (prev_adc_pot_bass - POT_HYST) || adc_pot_bass > (prev_adc_pot_bass + POT_HYST))
    {
        f_front_pot_bass_changed = true;
        front_pot_bass = adc_pot_bass;
    }

    prev_adc_pot_vol = adc_pot_vol;
    prev_adc_pot_treble = adc_pot_treble;
    prev_adc_pot_bass = adc_pot_bass;
}

void front_read_buttons()
{
    // Buttons
    if (digitalRead(BUTTON_ENCODER) < prev_button_encoder)
    {
        f_front_button_encoder_pressed = true;
    }

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