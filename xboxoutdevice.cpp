#include <fcntl.h>
#include <unistd.h>

#include "xboxoutdevice.h"

XboxOutDevice::XboxOutDevice(QObject *parent) : QObject(parent)
{
    uinputHandler = 0;
}

void XboxOutDevice::createDevice()
{
    uinputHandler = open("/dev/uinput", O_RDWR | O_NONBLOCK);
    struct uinput_user_dev uidev;
    strncpy(uidev.name, "Microsoft X-Box 360 pad", UINPUT_MAX_NAME_SIZE);
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x045e;
    uidev.id.product = 0x028e;
    uidev.id.version = 276;
    uidev.ff_effects_max = 0;

    long result = 0;
    result = ioctl(uinputHandler, UI_SET_EVBIT, EV_KEY);
    result = ioctl(uinputHandler, UI_SET_EVBIT, EV_SYN);
    result = ioctl(uinputHandler, UI_SET_EVBIT, EV_ABS);

    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_SOUTH);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_EAST);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_NORTH);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_WEST);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_TL);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_TR);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_SELECT);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_START);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_MODE);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_THUMBL);
    result = ioctl(uinputHandler, UI_SET_KEYBIT, BTN_THUMBR);

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_X);
    uidev.absmin[ABS_X] = -32768;
    uidev.absmax[ABS_X] = 32767;
    uidev.absfuzz[ABS_X] = 0;
    uidev.absflat[ABS_X] = 128;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_Y);
    uidev.absmin[ABS_Y] = -32768;
    uidev.absmax[ABS_Y] = 32767;
    uidev.absfuzz[ABS_Y] = 0;
    uidev.absflat[ABS_Y] = 128;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_RX);
    uidev.absmin[ABS_RX] = -32768;
    uidev.absmax[ABS_RX] = 32767;
    uidev.absfuzz[ABS_RX] = 16;
    uidev.absflat[ABS_RX] = 128;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_RY);
    uidev.absmin[ABS_RY] = -32768;
    uidev.absmax[ABS_RY] = 32767;
    uidev.absfuzz[ABS_RY] = 16;
    uidev.absflat[ABS_RY] = 128;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_Z);
    uidev.absmin[ABS_Z] = 0;
    uidev.absmax[ABS_Z] = 255;
    uidev.absfuzz[ABS_Z] = 0;
    uidev.absflat[ABS_Z] = 0;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_RZ);
    uidev.absmin[ABS_RZ] = 0;
    uidev.absmax[ABS_RZ] = 255;
    uidev.absfuzz[ABS_RZ] = 0;
    uidev.absflat[ABS_RZ] = 0;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_HAT0X);
    uidev.absmin[ABS_HAT0X] = -1;
    uidev.absmax[ABS_HAT0X] = 1;
    uidev.absfuzz[ABS_HAT0X] = 0;
    uidev.absflat[ABS_HAT0X] = 0;

    result = ioctl(uinputHandler, UI_SET_ABSBIT, ABS_HAT0Y);
    uidev.absmin[ABS_HAT0Y] = -1;
    uidev.absmax[ABS_HAT0Y] = 1;
    uidev.absfuzz[ABS_HAT0Y] = 0;
    uidev.absflat[ABS_HAT0Y] = 0;

    result = write(uinputHandler, &uidev, sizeof(uidev));
    result = ioctl(uinputHandler, UI_DEV_CREATE);
    Q_UNUSED(result);
}

void XboxOutDevice::relayReport(DS4State &report, DS4State &previous)
{
    struct input_event ev;
    struct input_event ev2;

    bool syn = false;
    checkBtnEvent(report.Cross, previous.Cross, BTN_SOUTH, ev, syn);
    checkBtnEvent(report.Circle, previous.Circle, BTN_EAST, ev, syn);
    checkBtnEvent(report.Triangle, previous.Triangle, BTN_WEST, ev, syn);
    checkBtnEvent(report.Square, previous.Square, BTN_NORTH, ev, syn);
    checkBtnEvent(report.L1, previous.L1, BTN_TL, ev, syn);
    checkBtnEvent(report.R1, previous.R1, BTN_TR, ev, syn);
    checkBtnEvent(report.Share, previous.Share, BTN_SELECT, ev, syn);
    checkBtnEvent(report.Options, previous.Options, BTN_START, ev, syn);
    checkBtnEvent(report.PS, previous.PS, BTN_MODE, ev, syn);
    checkBtnEvent(report.L3, previous.L3, BTN_THUMBL, ev, syn);
    checkBtnEvent(report.R3, previous.R3, BTN_THUMBR, ev, syn);

    checkAxisEvent(report.LX, previous.LX, ABS_X, false, ev, syn);
    checkAxisEvent(report.LY, previous.LY, ABS_Y, false, ev, syn);
    checkAxisEvent(report.RX, previous.RX, ABS_RX, false, ev, syn);
    checkAxisEvent(report.RY, previous.RY, ABS_RY, false, ev, syn);
    checkTriggerEvent(report.L2, previous.L2, ABS_Z, ev, syn);
    checkTriggerEvent(report.R2, previous.R2, ABS_RZ, ev, syn);

    // DPad Check
    short dpadx = report.dpad & DPAD_LEFT ? -1 : report.dpad & DPAD_RIGHT ? 1 : 0;
    short dpady = report.dpad & DPAD_UP ? -1 : report.dpad & DPAD_DOWN ? 1 : 0;
    short prevDpadx = previous.dpad & DPAD_LEFT ? -1 : previous.dpad & DPAD_RIGHT ? 1 : 0;
    short prevDpady = previous.dpad & DPAD_UP ? -1 : previous.dpad & DPAD_DOWN ? 1 : 0;
    checkDPadEvent(dpadx, prevDpadx, ABS_HAT0X, ev, syn);
    checkDPadEvent(dpady, prevDpady, ABS_HAT0Y, ev, syn);

    if (syn)
    {
        memset(&ev2, 0, sizeof(struct input_event));
        gettimeofday(&ev2.time, nullptr);
        ev2.type = EV_SYN;
        ev2.code = SYN_REPORT;
        ev2.value = 0;

        long result = 0;
        result = write(uinputHandler, &ev2, sizeof(struct input_event));
        Q_UNUSED(result);
    }
}

void inline XboxOutDevice::checkBtnEvent(bool curBtn, bool prevBtn, ushort outValue, struct input_event &ev, bool &syn)
{
    if (curBtn != prevBtn)
    {
        memset(&ev, 0, sizeof(struct input_event));
        gettimeofday(&ev.time, nullptr);
        ev.type = EV_KEY;
        ev.code = outValue;
        ev.value = curBtn ? 1 : 0;

        long result = 0;
        result = write(uinputHandler, &ev, sizeof(struct input_event));
        Q_UNUSED(result);
        syn = true;
    }
}

void inline XboxOutDevice::checkAxisEvent(ushort axisVal, ushort prevAxisVal, ushort outValue, bool flip,
                           struct input_event &ev, bool &syn)
{
    if (axisVal != prevAxisVal)
    {
        memset(&ev, 0, sizeof(struct input_event));
        gettimeofday(&ev.time, nullptr);
        ev.type = EV_ABS;
        ev.code = outValue;
        ev.value = axisScale(axisVal, flip);

        long result = 0;
        result = write(uinputHandler, &ev, sizeof(struct input_event));
        Q_UNUSED(result);
        syn = true;
    }
}

void inline XboxOutDevice::checkTriggerEvent(ushort axisVal, ushort prevAxisVal, ushort outValue,
                           struct input_event &ev, bool &syn)
{
    if (axisVal != prevAxisVal)
    {
        memset(&ev, 0, sizeof(struct input_event));
        gettimeofday(&ev.time, nullptr);
        ev.type = EV_ABS;
        ev.code = outValue;
        ev.value = axisVal;

        long result = 0;
        result = write(uinputHandler, &ev, sizeof(struct input_event));
        Q_UNUSED(result);
        syn = true;
    }
}

void inline XboxOutDevice::checkDPadEvent(short curValue, short prevValue, ushort outValue,
                           struct input_event &ev, bool &syn)
{
    if (curValue != prevValue)
    {
        memset(&ev, 0, sizeof(struct input_event));
        gettimeofday(&ev.time, nullptr);
        ev.type = EV_ABS;
        ev.code = outValue;
        ev.value = curValue;

        long result = 0;
        result = write(uinputHandler, &ev, sizeof(struct input_event));
        Q_UNUSED(result);
        syn = true;
    }
}

short XboxOutDevice::axisScale(ushort value, bool flip)
{
    double maxZone = value >= 128 ? 127 : -128;
    short maxXOut = value >= 128 ? 32767 : -32768;
    double temp = (value - 128) / maxZone;
    if (flip) temp = (temp - 0.5) * -1.0 + 0.5;
    short out = static_cast<short>(temp * maxXOut);
    return out;
}

void XboxOutDevice::closeDevice()
{
    if (uinputHandler)
    {
        long result = 0;
        result = ioctl(uinputHandler, UI_DEV_DESTROY);
        result = close(uinputHandler);
        Q_UNUSED(result);
        uinputHandler = 0;
    }
}
