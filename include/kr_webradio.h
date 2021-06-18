#ifndef WEBRADIO_H
#define WEBRADIO_H

#include <Arduino.h>
//#include <WiFi.h>

void webradio_open_url(char * host, char * path);
void webradio_handle_stream(void);

#endif