#ifndef FRONTPANEL_H
#define FRONTPANEL_H

// Flags to signal change
extern bool f_front_pot_volume_changed;
extern bool f_front_pot_treble_changed;
extern bool f_front_pot_bass_changed;

extern bool f_front_button_encoder_pressed;

extern bool f_front_encoder_turn_left;
extern bool f_front_encoder_turn_right;

// Pot values
extern int front_pot_vol;
extern int front_pot_treble;
extern int front_pot_bass;


void front_read_pots();
void front_read_buttons();
void front_read_encoder();

#endif