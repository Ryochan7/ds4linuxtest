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
    void initBT11();
    void setOperational();

    const int MAX_READ_EVENTS = 2;
    static const int DS4_OUTPUT_REPORT_0x05_LEN = 32;
    static const int DS4_OUTPUT_REPORT_0x11_LEN = 78;
    //static const int DS4_OUTPUT_REPORT_0x11_LEN = 334;
    static const int DS4_INPUT_REPORT_0x11_LEN = 64;
    static const int DS4_OUTPUT_REPORT_0x15_LEN = 334;
    static const int DS4_FEATURE_REPORT_0x02_SIZE = 37;
    static const int DS4_FEATURE_REPORT_0x05_SIZE = 41;

    QFile *testDev;
    int hidHandle = 0;
    int hidWriteHandle = 0;
    int epfd = 0;
    int evfd = 0;
    struct epoll_event ev;
    //struct epoll_event ev1;
    struct epoll_event events[2];
    uchar bufshit[DS4_INPUT_REPORT_0x11_LEN];
    //uchar bufout[DS4_REPORT_0x05_LEN];
    uchar bufout[DS4_OUTPUT_REPORT_0x15_LEN]; // BT
    //uchar* bufout = NULL;
    //uchar *bufout = NULL;
    bool stillread = false;
    DS4State currentState;
    DS4State previousState;
    XboxOutDevice *outdev;
    QString devPath;
    bool started = false;

    QWaitCondition inputEnded;
    QMutex waitInputEnd;
    QThread testInputThread;
    QThread *btOutputThread = nullptr;
    QThread otherInputThread;
    //std::thread *dudebro;

signals:

public slots:
    void openDevice();
    void startShit();
    void stopDevice();
};

#endif // TESTER_H
