#include "udevdevmonitor.h"

UdevDevMonitor::UdevDevMonitor(QObject *parent) : QObject(parent)
{
    udev = udev_new();
}

bool UdevDevMonitor::start()
{
    return false;
}
