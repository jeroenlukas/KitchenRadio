#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <audioplayer/cbuf_ps.h>
#include "VS1053.h"


extern cbuf_ps circBuffer;
extern char readBuffer[4096] __attribute__((aligned(4)));

extern uint8_t mp3buff[32]; // vs1053 likes 32 bytes at a time

extern VS1053 player;

void audio_feedbuffer(int sound_mode);
void audio_getVS1053info();
void audio_flushbuffer();

#endif