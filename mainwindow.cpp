#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    backService = new ControlService();
    backService->moveToThread(&backThread);
    //testCol.moveToThread(&worker);
    connect(&backThread, SIGNAL(started()), backService, SLOT(startService()));
    //connect(&worker, SIGNAL(started()), &testCol, SLOT(startService()));
    //testfuck.moveToThread(&worker);
    //testfuck.openDevice();
    //connect(&worker, SIGNAL(started()), &testfuck, SLOT(startShit()));
    //connect(&worker, SIGNAL(finished()), &testfuck, SLOT(stopDevice()));
    backThread.start(QThread::NormalPriority);
}

MainWindow::~MainWindow()
{
    //testfuck.stopDevice();
    //testCol.stopControllers();
    backService->stopService();
    backService->quitMonitor();
    backThread.quit();
    backThread.wait();
    delete backService;
    delete ui;
}
