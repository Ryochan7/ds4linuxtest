#ifndef DEVICECOLLECTION_H
#define DEVICECOLLECTION_H

#include <libudev.h>
#include <QObject>
#include <QList>
#include <QHash>
#include <QString>

#include "tester.h"

class DeviceCollection : public QObject
{
    Q_OBJECT
public:
    explicit DeviceCollection(udev *udevinst, QObject *parent = nullptr);
    ~DeviceCollection();

    void findControllers();
    void stopControllers();
    void removeController(Tester* device);
    void clearControllers();

protected:
    udev_device *getDevChild(udev_device *parentdev, QString subsystem);

    QList<Tester*> controllers;
    QHash<QString, Tester*> existingDevPaths;
    struct udev *udevinst;

signals:

public slots:
    void startControllers();
};

#endif // DEVICECOLLECTION_H
