#include <Arduino.h>
#include "information/kr_info.h"

int info_items_int[INFO_CNT];
float info_items_float[INFO_CNT];
String info_items_string[INFO_CNT];

int info_get_int(int item)
{
    return info_items_int[item];
}

void info_set_int(int item, int value)
{
    info_items_int[item] = value;
}

void info_set_float(int item, float value)
{
    info_items_float[item] = value;
}

float info_get_float(int item)
{
    return info_items_float[item];
}

void info_set_string(int item, String value)
{
    info_items_string[item] = value;

}

String info_get_string(int item)
{
    return info_items_string[item];
}