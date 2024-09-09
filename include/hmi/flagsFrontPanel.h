#ifndef FLAGSFRONTPANEL_H
#define FLAGSFRONTPANEL_H

struct FlagsFrontPanel
{
    bool volumePotChanged;

    bool encoderButtonPressed;
    bool encoderTurnRight;
    bool encoderTurnLeft;

    bool buttonOffPressed;
    bool buttonRadioPressed;
    bool buttonBluetoothPressed;
    bool buttonSystemPressed;
    bool buttonAlarmPressed;
    bool buttonLampPressed;

};

#endif