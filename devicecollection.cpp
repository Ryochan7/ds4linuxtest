#include <libudev.h>
#include <QDebug>
#include <QListIterator>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>

#include "devicecollection.h"

struct vidInfo
{
    int vid;
    int pid;
};

static const int KNOWN_DEV_LEN = 2;
static struct vidInfo devInfo[KNOWN_DEV_LEN] = {
    {0x54c, 0xba0}, // Sony Wireless Adapter
    {0x54c, 0x54c}  // DS4 v.1
};

DeviceCollection::DeviceCollection(struct udev* udevinst, QObject *parent) : QObject(parent)
{
    this->udevinst = udevinst;
}

void DeviceCollection::findControllers()
{
    struct udev* udev = udevinst;
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;
    udev_list_entry_foreach(entry, devices)
    {
        const char* path = udev_list_entry_get_name(entry);
        struct udev_device* hidraw_dev = udev_device_new_from_syspath(udev, path);
        //struct udev_device *shitter = udev_device_get_parent(hidraw_dev);

        struct udev_device* usbhid
                    = udev_device_get_parent_with_subsystem_devtype(hidraw_dev, "usb", "usb_device");
        const char* path2 = udev_device_get_devpath(usbhid);
        Q_UNUSED(path2);
        const char* path3 = udev_device_get_devnode(hidraw_dev);

        QString tempPath =  QString::fromLocal8Bit(path3);
        if (existingDevPaths.contains(tempPath))
        {
            continue;
        }

        const char* vendor_id_str = udev_device_get_sysattr_value(usbhid, "idVendor");
        const char* product_id_str = udev_device_get_sysattr_value(usbhid, "idProduct");
        const char* serial = udev_device_get_sysattr_value(usbhid, "serial");
        qDebug() << serial;

        printf("block = %s, %s, usb = %s:%s, scsi = %s, shit = %s, ggg = %s\n",
                           path,
               udev_device_get_devnode(hidraw_dev),
                           udev_device_get_sysattr_value(usbhid, "idVendor"),
                           udev_device_get_sysattr_value(usbhid, "idProduct"),
                           udev_device_get_sysattr_value(hidraw_dev, "vendor"),
                udev_device_get_property_value(hidraw_dev, "DEVNAME"),
               path2)
                ;

        bool ok = false;
        int vendor_id = QString(vendor_id_str).toInt(&ok, 16);
        int product_id = QString(product_id_str).toInt(&ok, 16);

        bool foundDev = false;
        for (int ind = 0; ind < KNOWN_DEV_LEN && !foundDev; ind++)
        {
            vidInfo current = devInfo[ind];
            if (vendor_id == current.vid && product_id == current.pid)
            {
                //struct udev_device *usbtemp = udev_device_get_parent(usbhid);
                struct udev_device *chid = getDevChild(usbhid, "input");
                qDebug() << "DDD " << udev_device_get_property_value(chid, "idVendor");

                Tester *tempDev = new Tester(path3);
                controllers.append(tempDev);
                existingDevPaths.insert(tempPath, tempDev);
                qDebug() << "FOUND DEVICE";
                udev_device_unref(hidraw_dev);
                foundDev = true;
            }
        }

        if (!foundDev)
        {
            udev_device_unref(hidraw_dev);
        }
    }

    udev_enumerate_unref(enumerate);
}

void DeviceCollection::startControllers()
{
    QListIterator<Tester*> iter(controllers);
    while (iter.hasNext())
    {
        Tester *current = iter.next();
        if (!current->isStarted())
        {
            current->openDevice();
            //QtConcurrent::run(current, &Tester::startShit);
            current->startInput();
            //current->startShit();
        }
    }
}

void DeviceCollection::removeController(Tester *device)
{
    existingDevPaths.remove(device->getDevPath());
    controllers.removeAll(device);
}

void DeviceCollection::stopControllers()
{
    QListIterator<Tester*> iter(controllers);
    while (iter.hasNext())
    {
        Tester *current = iter.next();
        current->stopDevice();
    }
}

void DeviceCollection::clearControllers()
{
    QListIterator<Tester*> iter(controllers);
    while (iter.hasPrevious())
    {
        Tester *current = iter.previous();
        delete current;
    }

    controllers.clear();
}

struct udev_device* DeviceCollection::getDevChild(struct udev_device *parentdev, QString subsystem)
{
    QByteArray tempba = subsystem.toLatin1();
    const char* tempsubsys = tempba.data();
    struct udev_device *childdev = nullptr;
    struct udev_enumerate *enumerator = udev_enumerate_new(udevinst);
    udev_enumerate_add_match_parent(enumerator, parentdev);
    udev_enumerate_add_match_subsystem(enumerator, tempsubsys);
    udev_enumerate_scan_devices(enumerator);
    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerator);
    struct udev_list_entry *entry;
    udev_list_entry_foreach(entry, devices)
    {
        const char* path = udev_list_entry_get_name(entry);
        struct udev_device* hidraw_dev = udev_device_new_from_syspath(udevinst, path);
        qDebug() << "FUCK PATH " << udev_device_get_devnode(hidraw_dev);
        childdev = udev_device_new_from_syspath(udevinst, path);
        //break;
    }

    udev_enumerate_unref(enumerator);
    return childdev;
}

DeviceCollection::~DeviceCollection()
{
    controllers.clear();
}