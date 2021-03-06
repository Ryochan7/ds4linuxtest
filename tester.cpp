#include <fcntl.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <linux/hidraw.h>
//#include <linux/byteorder/little_endian.h>
#include "crc32.h"
//#include <zlib.h>

#include <QDebug>
#include <QTime>
#include <QApplication>
#include <QAbstractEventDispatcher>

#include "tester.h"

Tester::Tester(QObject *parent) : QObject(parent)
{
    //testDev = new QFile("/dev/hidraw4", this);
    //outdev = new XboxOutDevice(this);
}

Tester::Tester(const char* devpath, QObject *parent) : QObject(parent)
{
    this->devPath = QString::fromLocal8Bit(devpath);
    //fucker = new inner(this);
}

QString Tester::getDevPath()
{
    return devPath;
}

void Tester::openDevice()
{

    qDebug() << "Halls of justice painted green";

    //hidHandle = open("/dev/hidraw4", O_RDWR);

    tempThread = new QThread();
    // Run slot on new thread
    connect(tempThread, &QThread::started, this, &Tester::openHelper, Qt::DirectConnection);
    tempThread->start(QThread::HighPriority);
    tempThread->wait();

//    QByteArray ba = devPath.toLatin1();
//    const char* temppath = ba.data();
//    qDebug() << "OPATHIT " << temppath;
//    hidHandle = open(temppath, O_RDONLY);
//    hidWriteHandle = open(temppath, O_RDWR | O_NONBLOCK);
    //hidWriteHandle = open(temppath, O_WRONLY | O_NONBLOCK);

    //hidHandle = open("/dev/hidraw4", O_RDONLY);
    //hidWriteHandle = open("/dev/hidraw4", O_WRONLY | O_NONBLOCK);
    /*epfd = epoll_create(MAX_READ_EVENTS);

    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    ev.data.fd = hidHandle;
    epoll_ctl(epfd, EPOLL_CTL_ADD, hidHandle, &ev);

    evfd = eventfd(0, O_NONBLOCK);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = evfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, evfd, &ev);
    */
    //testDev->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
    //initDev();

    outdev = new XboxOutDevice(this);
    outdev->createDevice();
    started = true;
    //sleep(2);
}

void Tester::openHelper()
{
    qDebug() << "HALKJDLKJFLKJDLK";
    QByteArray ba = devPath.toLatin1();
    const char* temppath = ba.data();
    qDebug() << "OPATHIT " << temppath;
    hidHandle = open(temppath, O_RDONLY);
    hidWriteHandle = open(temppath, O_WRONLY | O_NONBLOCK);

    initDev();

//    outdev = new XboxOutDevice(this);
//    outdev->createDevice();

    tempThread->quit();
}

void Tester::initDev()
{
    if (conType == BT)
    {
        //bufout = new uchar[DS4_OUTPUT_REPORT_0x15_LEN];
        //for (int i = 0; i < DS4_OUTPUT_REPORT_0x15_LEN; i++) bufout[i] = 0;
        memset(bufout, 0, DS4_OUTPUT_REPORT_0x15_LEN);
        setOperational();
        initBT();
    }
    else
    {
        //bufout = new uchar[DS4_OUTPUT_REPORT_0x05_LEN];
        //for (int i = 0; i < DS4_OUTPUT_REPORT_0x05_LEN; i++) bufout[i] = 0;
        memset(bufout, 0, DS4_OUTPUT_REPORT_0x05_LEN);
        setOperational();
        initUSB();
    }
}

void Tester::initUSB()
{
    bufout[0] = 0x05;
    // enable rumble (0x01), lightbar (0x02), flash (0x04)
    bufout[1] = 0xf7;
    bufout[2] = 0x04;
    bufout[4] = 0x00; // fast motor
    bufout[5] = 0x00; // slow motor
    bufout[6] = 0x00; // red
    bufout[7] = 0xff; // green
    bufout[8] = 0x00; // blue
    bufout[9] = 0x00; // flash on duration
    bufout[10] = 0x00; // flash off duration

    //write(hidHandle, bufout, 32);
    //QTime start;
    //start.start();
    write(hidWriteHandle, bufout, DS4_OUTPUT_REPORT_0x05_LEN);

    //testDev->write((char*)bufout, 32);
    //qDebug() << "gdgdfdf " << start.elapsed();
}

void Tester::setOperational()
{
    // Getting calibration report puts the DS4 into PS4 mode
    int report_len = conType == BT ? DS4_FEATURE_REPORT_0x05_SIZE : DS4_FEATURE_REPORT_0x02_SIZE;
    uchar report[report_len];
    memset(report, 0, report_len);
    report[0] = conType == BT ? 0x05 : 0x02;

    int res = ioctl(hidWriteHandle, HIDIOCGFEATURE(report_len), &report);
    if (res >= 0)
    {
        qDebug() << "SUCCESS";
    }
}

void Tester::initBT()
{
    bufout[0] = 0x15;
    bufout[1] = 0xC0;
    bufout[2] = 0xA0;
    // enable rumble (0x01), lightbar (0x02), flash (0x04)
    bufout[3] = 0xF7;
    bufout[4] = 0x04;
    bufout[6] = 0x00; // fast motor
    bufout[7] = 0x00; // slow motor
    bufout[8] = 0x00; // red
    bufout[9] = 0xFF; // green
    bufout[10] = 0xFF; // blue
    bufout[11] = 0x00; // flash on duration
    bufout[12] = 0x00; // flash off duration

    //write(hidHandle, bufout, 32);

    /* CRC generation */
    uint8_t bthdr = 0xA2;
    uint32_t crc;

    // Place CRC32 data in report so interrupt endpoint can be used
    crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
    crc = ~crc32_le(crc, bufout, DS4_OUTPUT_REPORT_0x15_LEN-4);
    bufout[330] = crc;
    bufout[331] = crc >> 8;
    bufout[332] = crc >> 16;
    bufout[333] = crc >> 24;
    //__put_unaligned_le32(crc, &bufout[74]);

    int res = write(hidWriteHandle, bufout, DS4_OUTPUT_REPORT_0x15_LEN);
    qDebug() << res;
    Q_UNUSED(res);

    qDebug() << "gdgdfdf ";
}

void Tester::initBT11()
{
    bufout[0] = 0x11;
    bufout[1] = 0xC0;
    //bufout[2] = 0xA2;
    // enable rumble (0x01), lightbar (0x02), flash (0x04)
    bufout[3] = 0xF7;
    bufout[6] = 0x00; // fast motor
    bufout[7] = 0x00; // slow motor
    bufout[8] = 0x00; // red
    bufout[9] = 0xFF; // green
    bufout[10] = 0xFF; // blue
    bufout[11] = 0x00; // flash on duration
    bufout[12] = 0x00; // flash off duration

    //write(hidHandle, bufout, 32);

    /* CRC generation */
    uint8_t bthdr = 0xA2;
    uint32_t crc;

    crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
    crc = ~crc32_le(crc, bufout, DS4_OUTPUT_REPORT_0x11_LEN-4);
    bufout[74] = crc;
    bufout[75] = crc >> 8;
    bufout[76] = crc >> 16;
    bufout[77] = crc >> 24;
    //__put_unaligned_le32(crc, &bufout[74]);

    int res = write(hidWriteHandle, bufout, DS4_OUTPUT_REPORT_0x11_LEN);
    qDebug() << res;
    Q_UNUSED(res);
    //QTime start;
    //start.start();
    //testDev->write((char*)bufout, 32);
    //qDebug() << "gdgdfdf " << start.elapsed();
}

void Tester::startInput()
{
    moveToThread(&otherInputThread);
    connect(&otherInputThread, &QThread::started, this, &Tester::startShit);
    otherInputThread.start(QThread::HighPriority);

    //moveToThread(&testInputThread);
    //connect(&testInputThread, &QThread::started, this, &Tester::startShit);
    //testInputThread.start(QThread::HighPriority);
    //fucker->start(QThread::HighPriority);
    /*dudebro = new std::thread([this](){
        this->startShit();
    });
    */
}

static bool runevloop = false;
void Tester::startShit()
{
    stillread = true;
    memset(&bufshit, 0, DS4_INPUT_REPORT_0x11_LEN);
    int tempBattery = 0;
    Q_UNUSED(tempBattery);
    bool charging = false;
    Q_UNUSED(charging);
    static int maxBatteryLife = 11;
    int numEvents = 0;
    int currentEvent;
    long numbytes = 0;
    int offset = conType == BT ? 2 : 0;
    QAbstractEventDispatcher *shitfuck = QAbstractEventDispatcher::instance();
    thread()->msleep(100);

    qDebug() << "IN EVENT LOOP";

    /*while (true)
    {
        initBT();
    }
    */

    //while (stillread && testDev->read((char*)bufshit, 64))
    while (stillread)
    {
        //numEvents = epoll_wait(epfd, events, MAX_READ_EVENTS, 3000);
        //if (numEvents < 0)
        //{
        //    stillread = false;
        //    break;
        //}

        numbytes = read(hidHandle, bufshit, DS4_INPUT_REPORT_0x11_LEN);
        if (numbytes < 0)
        {
            stillread = false;
            break;
        }

        //qDebug() << "Shit after that";
        //qDebug() << bufshit[1];

        int ind = offset + 1;
        currentState.LX = bufshit[ind]; ind++;
        currentState.LY = bufshit[ind]; ind++;
        currentState.RX = bufshit[ind]; ind++;
        currentState.RY = bufshit[ind]; ind += 4;
        currentState.L2 = bufshit[ind]; ind++;
        currentState.R2 = bufshit[ind]; ind++;

        uchar tempByte = bufshit[5+offset];
        currentState.Triangle = (tempByte & (1 << 7)) != 0;
        currentState.Circle = (tempByte & (1 << 6)) != 0;
        currentState.Cross = (tempByte & (1 << 5)) != 0;
        currentState.Square = (tempByte & (1 << 4)) != 0;

        uchar dpadState = tempByte & 0x0F;
        switch (dpadState)
        {
            case 0: currentState.dpad = DPAD_UP; break;
        case 1: currentState.dpad = DPAD_UP | DPAD_RIGHT; break;
        case 2: currentState.dpad = DPAD_RIGHT; break;
        case 3: currentState.dpad = DPAD_DOWN | DPAD_RIGHT; break;
        case 4: currentState.dpad = DPAD_DOWN; break;
        case 5: currentState.dpad = DPAD_DOWN | DPAD_LEFT; break;
        case 6: currentState.dpad = DPAD_LEFT; break;
        case 7: currentState.dpad = DPAD_UP | DPAD_LEFT; break;
        case 8:
        default:
            currentState.dpad = DPAD_CENTER;
            break;
        }

        tempByte = bufshit[6+offset];
        currentState.R3 = (tempByte & (1 << 7)) != 0;
        currentState.L3 = (tempByte & (1 << 6)) != 0;
        currentState.Options = (tempByte & (1 << 5)) != 0;
        currentState.Share = (tempByte & (1 << 4)) != 0;
        currentState.R2Btn = (tempByte & (1 << 3)) != 0;
        currentState.L2Btn = (tempByte & (1 << 2)) != 0;
        currentState.R1 = (tempByte & (1 << 1)) != 0;
        currentState.L1 = (tempByte & (1 << 0)) != 0;

        tempByte = bufshit[7+offset];
        currentState.PS = (tempByte & (1 << 0)) != 0;
        currentState.TouchButton = (tempByte & 0x02) != 0;

        tempByte = bufshit[30+offset];
        charging = (tempByte & 0x10) != 0;
        currentState.Battery = static_cast<ushort>(qMin((tempByte & 0x0f) * 100 / maxBatteryLife, 100));


        //qDebug() << currentState.Square;

        outdev->relayReport(currentState, previousState);
        //copyState(currentState, previousState);
        previousState = currentState;

        /*for (currentEvent = 0; currentEvent < numEvents; currentEvent++)
        {
            if (events[currentEvent].data.fd == evfd)
            {
                stillread = false;
                break;
            }
            else if (events[currentEvent].data.fd == hidHandle)
            {

            }
        }
        */
        //QCoreApplication::processEvents();
        if (runevloop)
        //if (shitfuck->hasPendingEvents())
        {
            shitfuck->processEvents(QEventLoop::ProcessEventsFlag::ExcludeUserInputEvents |
                                    QEventLoop::ProcessEventsFlag::ExcludeSocketNotifiers |
                                    QEventLoop::ProcessEventsFlag::EventLoopExec |
                                    QEventLoop::ProcessEventsFlag::X11ExcludeTimers);
        }
    }

    //waitInputEnd.lock();
    //inputEnded.wakeAll();
    //waitInputEnd.unlock();

    qDebug() << "ENDED";
}

void Tester::stopDevice()
{
    //waitInputEnd.lock();
    stillread = false;
    //eventfd_write(evfd, 1);
    //close(epfd);
    if (hidHandle)
    {
        close(hidHandle);
        hidHandle = 0;
    }

    if (hidWriteHandle)
    {
        close(hidWriteHandle);
        hidWriteHandle = 0;
    }

    //inputEnded.wait(&waitInputEnd);
    //testDev->close();

    //fucker->quit();
    //fucker->wait();

    otherInputThread.quit();
    otherInputThread.wait();

    if (outdev)
    {
        outdev->closeDevice();
        //outdev = nullptr;
    }


    //dudebro->join();
    //testInputThread.quit();
    //testInputThread.wait();
    //waitInputEnd.unlock();
}

void Tester::copyState(DS4State &src, DS4State &dest)
{
    dest.LX = src.LX;
    dest.LY = src.LY;
    dest.RX = src.RX;
    dest.RY = src.RY;
    dest.L2 = src.L2;
    dest.R2 = src.R2;
    dest.dpad = src.dpad;
    dest.Cross = src.Cross;
    dest.Circle = src.Circle;
    dest.Triangle = src.Triangle;
    dest.Square = src.Square;
    dest.L1 = src.L1;
    dest.R1 = src.R1;
    dest.PS = src.PS;
    dest.Share = src.Share;
    dest.Options = src.Options;
    dest.L3 = src.L3;
    dest.R3 = src.R3;

}

bool Tester::isStarted()
{
    return started;
}

Tester::~Tester()
{
    if (btOutputThread)
    {
        delete btOutputThread;
        btOutputThread = nullptr;
    }

//    if (bufout)
//    {
//        delete [] bufout;
//        bufout = NULL;
//    }
}
