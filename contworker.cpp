#include "contworker.h"

ContWorker::ContWorker(Tester *controller, QObject *parent) : QObject(parent)
{
    this->controller = controller;
    controller->moveToThread(&worker);
    connect(&worker, SIGNAL(started()), controller, SLOT(startShit()));
}

void ContWorker::start()
{
    worker.start(QThread::HighPriority);
}

void ContWorker::stop()
{
    controller->stopDevice();
    worker.quit();
    worker.wait();
}
