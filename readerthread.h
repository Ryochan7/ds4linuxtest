#ifndef READERTHREAD_H
#define READERTHREAD_H

#include <QThread>

class Tester;

class ReaderThread : public QThread
{
    Q_OBJECT
public:
    ReaderThread(QObject *parent = nullptr);
    void setDev(Tester *test);

protected:
    void run() override;

    Tester *device;
};

#endif // READERTHREAD_H
