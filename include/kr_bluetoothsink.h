#ifndef BLUETOOTHSINK_H
#define BLUETOOTHSINK_H

#include <Arduino.h>
#include <BluetoothA2DPSink.h>

#include "kr_audioplayer.h"


extern BluetoothA2DPSink a2dp_sink;

extern bool f_bluetoothsink_metadata_received;

//extern unsigned char bt_wav_header[44];

extern char bluetooth_media_title[255];  

void bluetoothsink_avrc_metadata_callback(uint8_t data1, const uint8_t *data2);
void bluetoothsink_handle_stream();
void bluetoothsink_read_data_stream(const uint8_t *data, uint32_t length);
void bluetoothsink_end();
void bluetoothsink_start();
void bluetoothsink_setup();

void bluetoothsink_previous();
void bluetoothsink_next();


#endif