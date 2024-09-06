
#include <Arduino.h>
#include <BluetoothA2DPSink.h>

#include "audioplayer/krAudioPlayer.h"

#include "bluetoothsink/krBluetoothSink.h"

BluetoothA2DPSink a2dp_sink;

esp_a2d_audio_state_t current_state;

unsigned char bt_wav_header[44] = {
    0x52, 0x49, 0x46, 0x46, // RIFF
    0xFF, 0xFF, 0xFF, 0xFF, // size
    0x57, 0x41, 0x56, 0x45, // WAVE
    0x66, 0x6d, 0x74, 0x20, // fmt
    0x10, 0x00, 0x00, 0x00, // subchunk1size
    0x01, 0x00,             // audio format - pcm
    0x02, 0x00,             // numof channels
    0x44, 0xac, 0x00, 0x00, //, //samplerate 44k1: 0x44, 0xac, 0x00, 0x00       48k: 48000: 0x80, 0xbb, 0x00, 0x00,
    0x10, 0xb1, 0x02, 0x00, //byterate
    0x04, 0x00,             // blockalign
    0x10, 0x00,             // bits per sample - 16
    0x64, 0x61, 0x74, 0x61, // subchunk3id -"data"
    0xFF, 0xFF, 0xFF, 0xFF  // subchunk3size (endless)
};

bool f_bluetoothsink_metadata_received = false;

char bluetooth_media_title[255];

int cnt = 0;
void bluetoothsink_avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
a2dp_sink.set_auto_reconnect(true,  1000);
    Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);

    if (data1 == 0x1)
    {
        // Title
        strncpy(bluetooth_media_title, (char *)data2, sizeof(bluetooth_media_title) - 1);
    }
    else if (data1 == 0x2)
    {
        strncat(bluetooth_media_title, " - ", sizeof(bluetooth_media_title) - 1);
        strncat(bluetooth_media_title, (char *)data2, sizeof(bluetooth_media_title) - 1);
        f_bluetoothsink_metadata_received = true;
    }
}

void bluetoothsink_handle_stream() // not needed anymore, replaced by audio_feedbuffer()
{
    if (circBuffer.available())
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

void bluetoothsink_read_data_stream(const uint8_t *data, uint32_t length)
{

    //int bytes_read_from_stream = length;

    if (circBuffer.room() > length)
    {
        // If we get -1 here it means nothing could be read from the stream
        if (length > 0)
        {
            // Add them to the circular buffer
            circBuffer.write((char *)data, length); // length seems to be 4096 every time
            //Serial.printf("\nRead %lu bytes", length);
        }
    }
}

void audio_state_changed(esp_a2d_audio_state_t state, void* ptr) {
    Serial.println(a2dp_sink.to_str(state));
    current_state = state;
}

void bluetoothsink_setup()
{
    a2dp_sink.set_stream_reader(bluetoothsink_read_data_stream, false);
    a2dp_sink.set_avrc_metadata_callback(bluetoothsink_avrc_metadata_callback);
    //esp_a2d_connection_state_t state = a2dp_sink.get_connection_state();
   // a2dp_sink.set_auto_reconnect(true,  1000);
    a2dp_sink.set_on_audio_state_changed(audio_state_changed);
}

void bluetoothsink_start()
{
    //    a2dp_sink.set_auto_reconnect(true,  1000);
    //a2dp_sink.set_on_audio_state_changed(audio_state_changed);
    a2dp_sink.start("KitchenRadio", false);

    //a2dp_sink.reconnect();

    circBuffer.flush();
    /*if(a2dp_sink.get_last_peer_address()[0] != 0)
        {
            a2dp_sink.reconnect();
        }*/

    delay(100);
    circBuffer.write((char *)bt_wav_header, 44);
    delay(100);
}

void bluetoothsink_end()
{
    a2dp_sink.disconnect();
    a2dp_sink.end(false);
}

void bluetoothsink_next()
{
    a2dp_sink.next();
}

void bluetoothsink_previous()
{
    a2dp_sink.previous();
}

void bluetoothsink_playpause()
{
    if(a2dp_sink.is_connected())
    {   
    
        switch(a2dp_sink.get_audio_state())
        {
            case ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND:
                a2dp_sink.play();
                break;
            case ESP_A2D_AUDIO_STATE_STARTED:
                a2dp_sink.pause();
                //a2dp_sink.
                break;
            case ESP_A2D_AUDIO_STATE_STOPPED:
                // ?
                a2dp_sink.play();
            break;
        }
    }
   
}