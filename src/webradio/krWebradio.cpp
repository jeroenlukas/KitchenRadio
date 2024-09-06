#include <WiFi.h>
#include "webradio/krWebradio.h"
#include "audioplayer/krAudioPlayer.h"
#include "configuration/configMisc.h"


WiFiClient webradio_client;

bool dataPanic = false;

bool bufferedEnough = false;

bool webradio_isconnected()
{
    return (webradio_client.connected() > 0);
}

void webradio_open_url(char *host, char *path)
{
    if (webradio_client.connect(host, 80))
    {
        Serial.println("Connected now");
    }

    Serial.print(host);
    Serial.println(path);

    circBuffer.flush();

    bufferedEnough = false;

    webradio_client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n\r\n");
}

bool webradio_buffered_enough(void)
{
    return bufferedEnough;
}

void webradio_handle_stream(void)
{
    if (webradio_client.available())
    {
        int bytes_read_from_stream = 0;

        #define BYTESTOGET  512

        if (circBuffer.room() > BYTESTOGET)
        {
          //  if(webradio_client.available() > 1000) Serial.printf("More than 1000 bytes available: %d", webradio_client.available());
            // Read either the maximum available (max 100) or the number of bytes to the next meata data interval
            bytes_read_from_stream = webradio_client.read((uint8_t *)readBuffer, min(BYTESTOGET, (int)webradio_client.available()));

            // If we get -1 here it means nothing could be read from the stream
            if (bytes_read_from_stream > 0)
            {
                // Add them to the circular buffer
                circBuffer.write(readBuffer, bytes_read_from_stream);

                // Some radio stations (eg BBC Radio 4!!!) limit the data to 92 bytes. Why?
                /*if (bytes_read_from_stream < 92 && bytesReadFromStream != bytesUntilmetaData)
                {
                    Serial.printf("Only wrote %db to circ buff\n", bytesReadFromStream);
                }*/

                if(circBuffer.available() > CONF_AUDIO_MIN_BYTES)
                {
                    bufferedEnough = true;
                }
            }
        }
        else
        {
            // There will be thousands of this message. Only for debugging.
            //Serial.println("Circ buff full.");
        }
    }
/*
    if (circBuffer.available())
    {
        // Does the VS1053 want any more data (yet)?
        if (player.data_request())
        {
            {
                // Read the data from the circuluar (ring) buffer
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
    }*/
    
    return;
}
