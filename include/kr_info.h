#ifndef KR_INFO_H
#define KR_INFO_H

#define INFO_UPTIME     0
#define INFO_AUDIO_CHANNELS 1
#define INFO_AUDIO_BITRATE  2
#define INFO_AUDIO_SAMPLERATE  3

#define INFO_WEATHER_TEMP       4
#define INFO_WEATHER_WIND_KMH   5

#define INFO_TIME_SHORT         6

#define INFO_CNT 7

int info_get_int(int item);
void info_set_int(int item, int value);

void info_set_float(int item, float value);
float info_get_float(int item);

void info_set_string(int item, String value);
String info_get_string(int item);

#endif