#include <Arduino.h>
#include "information/krInfo.h"

Information information;

void krInfoInitialize(void)
{
    information.timeShort = "00:00";

    information.system.uptimeSeconds = 0;
    information.system.wifiRSSI = 0;

    information.weather.temperature = 0.0;
}
