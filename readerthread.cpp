#include "readerthread.h"

#include "tester.h"

ReaderThread::ReaderThread(QObject *parent) : QThread (parent)
{
}

void ReaderThread::setDev(Tester *test)
{
    this->device = test;
}

void ReaderThread::run()
{
    exec();
    //device->startShit();
}
