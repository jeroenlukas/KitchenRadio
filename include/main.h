#ifndef MAIN_H
#define MAIN_H

#include "VS1053.h"


cbuf circBuffer(1024 * 20);
char readBuffer[100] __attribute__((aligned(4)));

uint8_t mp3buff[32]; // vs1053 likes 32 bytes at a time

VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);


#endif