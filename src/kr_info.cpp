#include <Arduino.h>
#include "kr_info.h"

int info_items_int[INFO_CNT];

int info_get_int(int item)
{
    return info_items_int[item];
}

int info_set_int(int item, int value)
{
    info_items_int[item] = value;
}