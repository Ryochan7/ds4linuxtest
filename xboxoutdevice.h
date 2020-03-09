#ifndef XBOXOUTDEVICE_H
#define XBOXOUTDEVICE_H

#include <linux/uinput.h>
#include <QObject>

#include "ds4report.h"

class XboxOutDevice : public QObject
{
    Q_OBJECT
public:
    explicit XboxOutDevice(QObject *parent = nullptr);
    void createDevice();
    void relayReport(DS4State &report, DS4State &previous);
    void closeDevice();

protected:
    void inline checkBtnEvent(bool curBtn, bool prevBtn, ushort outValue,
                              struct input_event &ev, bool &syn);

    void inline checkAxisEvent(ushort axisVal, ushort prevAxisVal, ushort outValue, bool flip,
                               struct input_event &ev, bool &syn);

    void inline checkTriggerEvent(ushort axisVal, ushort prevAxisVal, ushort outValue,
                               struct input_event &ev, bool &syn);

    void inline checkDPadEvent(short curValue, short prevValue, ushort outValue,
                               struct input_event &ev, bool &syn);

    short axisScale(ushort value, bool flip);

    int uinputHandler;

signals:

public slots:
};

#endif // XBOXOUTDEVICE_H
