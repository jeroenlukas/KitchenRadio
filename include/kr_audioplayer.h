#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <cbuf.h>
#include "VS1053.h"


extern cbuf circBuffer;
extern char readBuffer[1000] __attribute__((aligned(4)));

extern uint8_t mp3buff[32]; // vs1053 likes 32 bytes at a time

extern VS1053 player;

#endif