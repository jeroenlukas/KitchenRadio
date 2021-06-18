#ifndef KR_INFO_H
#define KR_INFO_H

#define INFO_UPTIME     0
#define INFO_AUDIO_CHANNELS 1
#define INFO_AUDIO_BITRATE  2
#define INFO_AUDIO_SAMPLERATE  3

#define INFO_CNT 4

int info_get_int(int item);
int info_set_int(int item, int value);

#endif