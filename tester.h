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
    enum ConnectionType {USB, BT, SONYWA};

    explicit Tester(QObject *parent = nullptr);
    ~Tester();
    Tester(const char* devpath, QObject *parent = nullptr);
    void copyState(DS4State &src, DS4State &dest);
    QString getDevPath();
    void startInput();
    bool isStarted();

    ConnectionType conType = ConnectionType::USB;

protected:
    void initDev();
    void initUSB();
    void initBT();
    void setOperational();

    const int MAX_READ_EVENTS = 2;
    static const int DS4_REPORT_0x05_LEN = 32;
    static const int DS4_REPORT_0x11_LEN = 78;
    static const int DS4_REPORT_0x15_LEN = 334;
    static const int DS4_FEATURE_REPORT_0x05_SIZE = 41;

    QFile *testDev;
    int hidHandle;
    int hidWriteHandle;
    int epfd;
    int evfd;
    struct epoll_event ev;
    //struct epoll_event ev1;
    struct epoll_event events[2];
    uchar bufshit[64];
    //uchar bufout[DS4_REPORT_0x05_LEN];
    uchar bufout[DS4_REPORT_0x11_LEN]; // BT
    //uchar *bufout = NULL;
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
