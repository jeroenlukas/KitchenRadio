#ifndef STATIONS_H
#define STATIONS_H

char * stations_name[] = { "NPO Radio 1", "NPO Radio 2", "BNR"};
char * stations_host[] = { "icecast.omroep.nl", "icecast.omroep.nl", "22533.live.streamtheworld.com" };
char * stations_path[] = { "/radio1-bb-mp3", "/radio2-bb-mp3", "/BNR_NIEUWSRADIO.mp3?dist=other" };

int stations_cnt = 3;

#endif
