#ifndef CONTWORKER_H
#define CONTWORKER_H

#include <QObject>
#include <QThread>

#include "tester.h"

class ContWorker : public QObject
{
    Q_OBJECT
public:
    explicit ContWorker(Tester *controller, QObject *parent = nullptr);
    void start();
    void stop();

protected:
    QThread worker;
    Tester *controller;

signals:

public slots:
};

#endif // CONTWORKER_H
