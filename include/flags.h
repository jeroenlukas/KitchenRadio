#ifndef FLAGS_H
#define FLAGS_H


#include "hmi/flagsFrontPanel.h"
/*
This file contains event flags, which can be set and read throughout the system to track events.
Components that fire events should contain a flags***.h file, which contains a struct containing the flags.
*/

class Flags
{
    public:

    FlagsFrontPanel flagsFrontPanel;

};

extern Flags flags;

#endif