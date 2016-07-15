#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QThread>
#include <QFileDialog>
#include <QFile>
#include "serialworker.h"
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void SerialPortConnected();
    void SerialPortDisconnected();
    void NewSerialMessage(QString);
    void SetStatusBarMessage(QString);
    void ProgressChange(int);
    void FinishedFileTransfer();
    void on__refreshPushButton_clicked();
    void on__connectPushButton_clicked();
    void on__sendPushButton_clicked();
    void on__commandLineEdit_returnPressed();
    void on__transferPushButton_clicked();
    void on__selectFilePushButton_clicked();

private:
    void RefreshPorts();
    void SerialConnectedUI(bool SetEnable);

private:
    Ui::MainWindow *ui;
    QThread *_thread;
    SerialWorker *_serialWorker;
    bool _isConnected;
};

#endif // MAINWINDOW_H
