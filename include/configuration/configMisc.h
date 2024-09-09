#ifndef CONFIGMISC_H
#define CONFIGMISC_H

#define FORMAT_LITTLEFS_IF_FAILED true

// Volume pot hysteresis
#define POT_HYST    1

// Circular buffer size in PSRAM
#define CIRCBUFFER_SIZE 2 * 1024 * 1024 // 2 MB

// Minimum no. of bytes required in circular buffer to play
#define CONF_AUDIO_MIN_BYTES    256 * 1024

#define DISPLAY_RETURN_TIME    20 //*100 ms

// Multibutton ranges
#define BTN_ADC_OFF_MIN         3400
#define BTN_ADC_OFF_MAX         3500
#define BTN_ADC_RADIO_MIN       2700
#define BTN_ADC_RADIO_MAX       2800
#define BTN_ADC_BLUETOOTH_MIN   2150
#define BTN_ADC_BLUETOOTH_MAX   2200
#define BTN_ADC_SYSTEM_MIN      1550
#define BTN_ADC_SYSTEM_MAX      1650
#define BTN_ADC_ALARM_MIN       400
#define BTN_ADC_ALARM_MAX       450
#define BTN_ADC_LAMP_MIN        980
#define BTN_ADC_LAMP_MAX        1050

#endif
