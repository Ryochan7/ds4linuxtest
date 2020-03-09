#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "tester.h"
#include "devicecollection.h"
#include "controlservice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QThread backThread;
    Tester testfuck;
    //DeviceCollection testCol;
    ControlService *backService;
};

#endif // MAINWINDOW_H
