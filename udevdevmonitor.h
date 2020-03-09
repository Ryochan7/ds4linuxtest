#ifndef UDEVDEVMONITOR_H
#define UDEVDEVMONITOR_H

#include <libudev.h>
#include <QObject>

class UdevDevMonitor : public QObject
{
    Q_OBJECT
public:
    explicit UdevDevMonitor(QObject *parent = nullptr);
    bool start();

protected:
    struct udev* udev;

signals:

public slots:
};

#endif // UDEVDEVMONITOR_H
