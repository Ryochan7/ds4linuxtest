#ifndef CONTROLSERVICE_H
#define CONTROLSERVICE_H

#include <libudev.h>
#include <QObject>

#include "devicecollection.h"

class ControlService : public QObject
{
    Q_OBJECT
public:
    explicit ControlService(QObject *parent = nullptr);
    ~ControlService();
    void quitMonitor();

protected:
    void hotplug();
    void setupHotplug();

    DeviceCollection *contCollect;
    bool runHotplug;
    struct udev *udevinst;
    bool runMonitor;
    struct epoll_event ev;
    struct epoll_event events[2];
    int epfd;
    int evfd;

signals:

public slots:
    void startService();
    void stopService();
};

#endif // CONTROLSERVICE_H
