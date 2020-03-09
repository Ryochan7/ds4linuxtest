#include <fcntl.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <QtConcurrent/QtConcurrent>


#include "controlservice.h"

ControlService::ControlService(QObject *parent) : QObject(parent)
{
    udevinst = udev_new();
    contCollect = new DeviceCollection(udevinst, this);

    QtConcurrent::run(this, &ControlService::setupHotplug);
}

void ControlService::startService()
{
    contCollect->findControllers();
    contCollect->startControllers();
    runHotplug = true;
}

void ControlService::stopService()
{
    runHotplug = false;
    contCollect->stopControllers();
    contCollect->clearControllers();
}

void ControlService::hotplug()
{
    qDebug() << "ATTEMPT HOTPLUG";
    if (runHotplug)
    {
        contCollect->findControllers();
        contCollect->startControllers();
    }
}

void ControlService::setupHotplug()
{
    struct udev_monitor* mon = udev_monitor_new_from_netlink(udevinst, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device");
    udev_monitor_enable_receiving(mon);
    int fd = udev_monitor_get_fd(mon);
    epfd = epoll_create(2);

    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    evfd = eventfd(0, O_NONBLOCK);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = evfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, evfd, &ev);

    Q_UNUSED(fd);
    runMonitor = true;
    while (runMonitor)
    {
        int numEvents =  epoll_wait(epfd, events, 2, -1);
        for (int i = 0; i < numEvents; i++)
        {
            if (events[i].data.fd == fd)
            {
                struct udev_device *device = udev_monitor_receive_device(mon);
                udev_device_unref(device);
                QTimer::singleShot(0, this, &ControlService::hotplug);
            }
            else if (events[i].data.fd == evfd)
            {
                runMonitor = false;
            }
        }
    }
}

void ControlService::quitMonitor()
{
    if (evfd)
    {
        eventfd_write(evfd, 1);
    }
}

ControlService::~ControlService()
{
    if (evfd)
    {
        if (runMonitor)
        {
            eventfd_write(evfd, 1);
            sleep(1);
        }

        close(evfd);
        evfd = 0;
    }

    if (epfd)
    {
        close(epfd);
        epfd = 0;
    }

    if (udevinst)
    {
        udev_unref(udevinst);
        udevinst = nullptr;
    }
}
