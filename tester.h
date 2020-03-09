#ifndef TESTER_H
#define TESTER_H

#include <sys/epoll.h>

#include <QObject>
#include <QFile>
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include <QTimer>

#include "readerthread.h"
#include "ds4report.h"
#include "xboxoutdevice.h"

class Tester : public QObject
{
    Q_OBJECT
public:
    explicit Tester(QObject *parent = nullptr);
    ~Tester();
    Tester(const char* devpath, QObject *parent = nullptr);
    void copyState(DS4State &src, DS4State &dest);
    QString getDevPath();
    void startInput();
    bool isStarted();

protected:
    void initDev();

    const int MAX_READ_EVENTS = 2;

    QFile *testDev;
    int hidHandle;
    int hidWriteHandle;
    int epfd;
    int evfd;
    struct epoll_event ev;
    //struct epoll_event ev1;
    struct epoll_event events[2];
    uchar bufshit[64];
    uchar bufout[32];
    bool stillread;
    DS4State currentState;
    DS4State previousState;
    XboxOutDevice *outdev;
    QString devPath;
    bool started;

    QWaitCondition inputEnded;
    QMutex waitInputEnd;
    QThread testInputThread;
    QThread *btOutputThread = nullptr;
    ReaderThread otherInputThread;
    //std::thread *dudebro;

signals:

public slots:
    void openDevice();
    void startShit();
    void stopDevice();
};

#endif // TESTER_H
