#include <Arduino.h>

#include <cbuf.h>
#include "VS1053.h"
#include "configPinout.h"
#include "kr_audioplayer.h"


cbuf circBuffer(1024 * 20); //64);
char readBuffer[1000] __attribute__((aligned(4)));

uint8_t mp3buff[32]; // vs1053 likes 32 bytes at a time

VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);