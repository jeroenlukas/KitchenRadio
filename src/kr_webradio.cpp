#include <WiFi.h>
#include "kr_webradio.h"
#include "kr_audioplayer.h"

WiFiClient webradio_client;

bool dataPanic = false;

void webradio_open_url(char *host, char *path)
{
    if (webradio_client.connect(host, 80))
    {
        Serial.println("Connected now");
    }

    Serial.print(host);
    Serial.println(path);

    circBuffer.flush();

    webradio_client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n\r\n");
}

void webradio_handle_stream(void)
{
    if (webradio_client.available())
    {
        int bytes_read_from_stream = 0;

        if (circBuffer.room() > 999)
        {

            // Read either the maximum available (max 100) or the number of bytes to the next meata data interval
            bytes_read_from_stream = webradio_client.read((uint8_t *)readBuffer, min(1000, (int)webradio_client.available()));

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
            }
        }
        else
        {
            // There will be thousands of this message. Only for debugging.
            //Serial.println("Circ buff full.");
        }
    }

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
    }
    else
    {
        if (!dataPanic)
        {
            //Serial.println("PANIC No audio data to read from circular buffer");
            // TODO: we might need to auto reconnect here?
            dataPanic = true;
        }
        else
        {
            dataPanic = false;
        }
    }
    /*  if (webradio_client.available() > 0) 
    {
        Serial.println("data avail");
        for(int i = 0; i < webradio_client.available(); i++)
        {
            circbuffer.unshift(webradio_client.read());            
        }        
    }

    if(circbuffer.size() > 32)
    {
        for(int i = 0; i < 32; i++)
        {
            mp3buff[i] = circbuffer.shift();
        }
        Serial.println("playchnk");
        player.playChunk(mp3buff, 32);
    }

    if(circbuffer.isEmpty())
        Serial.println("* Buffer is empty");
    else if(circbuffer.isFull())
        Serial.println("* Buffer is full!");*/

    /* unsigned int sample = analogRead(SAMPLE_PIN);
	if (sample != buffer.first()->value()) {
		Record* record = new Record(millis(), sample);
		buffer.unshift(record);
		Serial.println("---");
		delay(50);
	}
	if (buffer.isFull()) {
		Serial.println("Queue is full:");
		while (!buffer.isEmpty()) {
			Record* record = buffer.shift();
			record->print(&Serial);
			delete record;
			Serial.println();
		}
		Serial.println("START AGAIN");
	}*/

    /*
    if (webradio_client.available() > 0) 
    {
      uint8_t bytesread = webradio_client.read(mp3buff, 32);
      player.playChunk(mp3buff, bytesread);
    }*/
    return;
}
