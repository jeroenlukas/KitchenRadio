#include <Arduino.h>

//#include <cbuf.h>
#include "VS1053.h"
#include "configuration/config.h"
//#include "configuration/configMisc.h"
#include "configuration/constants.h"
#include "audioplayer/kr_audioplayer.h"
#include "audioplayer/cbuf_ps.h"
#include "information/kr_info.h"
#include "webradio/kr_webradio.h"

cbuf_ps circBuffer(1024); //64);
char readBuffer[4096] __attribute__((aligned(4)));

uint8_t mp3buff[32]; // vs1053 likes 32 bytes at a time

VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);

void IRAM_ATTR audio_feedbuffer(int sound_mode)
{
    if((sound_mode == SOUNDMODE_WEBRADIO) && (webradio_buffered_enough() == false))
        return; // Wait until enough bytes are in the buffer
    
    if (circBuffer.available() > 0)
    {
        // Does the VS1053 want any more data (yet)?
        if (player.data_request())
        {

            int bytesRead = circBuffer.read((char *)mp3buff, 32);
            
            // If we didn't read the full 32 bytes, that's a worry
            if (bytesRead != 32)
            {
                Serial.printf("Only read %d bytes from  circular buffer\n", bytesRead);
            }

            // Actually send the data to the VS1053
            player.playChunk(mp3buff, bytesRead);
        }
    }
}

void audio_flushbuffer()
{
    circBuffer.flush();
}

void audio_getVS1053info()
{
    int vs1053_AUDATA = player.read_register(0x5);
    info_set_int(INFO_AUDIO_SAMPLERATE, vs1053_AUDATA & 0xFFFE);
    info_set_int(INFO_AUDIO_CHANNELS, (vs1053_AUDATA & 1) + 1);
    info_set_int(INFO_AUDIO_BITRATE, player.get_bitrate());
}