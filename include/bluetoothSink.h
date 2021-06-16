#ifndef BLUETOOTHSINK_H
#define BLUETOOTHSINK_H

#include <Arduino.h>

BluetoothA2DPSink a2dp_sink;

bool f_bluetoothsink_metadata_received = false;

unsigned char bt_wav_header[] = {
    0x52, 0x49, 0x46, 0x46, // RIFF
    0xFF, 0xFF, 0xFF, 0xFF, // size
    0x57, 0x41, 0x56, 0x45, // WAVE
    0x66, 0x6d, 0x74, 0x20, // fmt
    0x10, 0x00, 0x00, 0x00, // subchunk1size
    0x01, 0x00,             // audio format - pcm
    0x02, 0x00,             // numof channels
    0x80, 0xbb, 0x00, 0x00, //, //samplerate 44k1: 0x44, 0xac, 0x00, 0x00       48k: 48000: 0x80, 0xbb, 0x00, 0x00,
    0x10, 0xb1, 0x02, 0x00, //byterate
    0x04, 0x00,             // blockalign
    0x10, 0x00,             // bits per sample - 16
    0x64, 0x61, 0x74, 0x61, // subchunk3id -"data"
    0xFF, 0xFF, 0xFF, 0xFF  // subchunk3size (endless)
};

char bluetooth_media_title[255];  

void bluetoothsink_avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
  
  Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);

  if(data1 == 0x1)
  {
    // Title
    strncpy(bluetooth_media_title, (char*)data2, sizeof(bluetooth_media_title)-1);
  }
  else if(data1 == 0x2)
  {
      strncat(bluetooth_media_title, " - ", sizeof(bluetooth_media_title)-1);
      strncat(bluetooth_media_title, (char*)data2, sizeof(bluetooth_media_title)-1);      
      f_bluetoothsink_metadata_received = true;
  }
}

void bluetoothsink_handle_stream()
{
  if (circBuffer.available())
  {
    // Does the VS1053 want any more data (yet)?
    if (player.data_request())
    {
      {
        /*if (first)
        {
          first = false;
          player.playChunk(bt_wav_header, 32);
          player.playChunk(bt_wav_header+32, 44-32);
          player.printDetails("info");
          Serial.printf("\n Media type: %d", a2dp_sink.get_audio_type());
        }
        else*/
        {
          // Read the data from the circuluar (ring) buffer
          int bytesRead = circBuffer.read((char *)mp3buff, 32);

          /*if (1)
        {

          for (int i = 0; i < bytesRead; i++)
            Serial.printf(" %x", mp3buff[i]);
          first = false;
        }*/

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
  }
}

void bluetoothsink_read_data_stream(const uint8_t *data, uint32_t length)
{
  //if (sound_mode == SOUNDMODE_BLUETOOTH)
  {
    int bytes_read_from_stream = length;

    if (circBuffer.room() > length)
    {

      // Read either the maximum available (max 100) or the number of bytes to the next meata data interval
      //bytes_read_from_stream = webradio_client.read((uint8_t *)readBuffer, min(100, (int)webradio_client.available()));

      // If we get -1 here it means nothing could be read from the stream
      if (bytes_read_from_stream > 0)
      {
        // Add them to the circular buffer
        circBuffer.write((char *)data, length);

        // Some radio stations (eg BBC Radio 4!!!) limit the data to 92 bytes. Why?
        /*if (bytes_read_from_stream < 92 && bytesReadFromStream != bytesUntilmetaData)
                {
                    Serial.printf("Only wrote %db to circ buff\n", bytesReadFromStream);
                }*/
      }
    }
  }
}

#endif