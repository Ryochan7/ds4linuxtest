#ifndef DS4REPORT_H
#define DS4REPORT_H

typedef enum _Buttons
{
    None = 0,
    Cross = 1 << 1,
} Buttons;

const unsigned int DPAD_CENTER = 0 << 0;
const unsigned int DPAD_UP = 1 << 0;
const unsigned int DPAD_RIGHT = 1 << 1;
const unsigned int DPAD_DOWN = 1 << 2;
const unsigned int DPAD_LEFT = 1 << 3;

typedef struct _DS4State
{
    unsigned int dpad = 0;
    unsigned int buttons = 0;
    bool Cross = false;
    bool Circle = false;
    bool Triangle = false;
    bool Square = false;
    bool L1 = false;
    bool R1 = false;
    bool Options, Share, PS = false;
    bool L2Btn, R2Btn = false;
    bool L3, R3 = false;
    bool TouchButton = false;
    unsigned short LX = 128;
    unsigned short LY = 128;
    unsigned short RX = 128;
    unsigned short RY = 128;
    unsigned short L2 = 0;
    unsigned short R2 = 0;
    unsigned short Battery = 0;

} DS4State;

#endif // DS4REPORT_H
