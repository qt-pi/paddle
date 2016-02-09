#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDataStream>
#include <QProcess>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QScrollBar>
#include <QTime>
#include <QThread>
#include "serialworker.h"

namespace Ui {
class MainWorker;
}

class MainWorker : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWorker(QWidget *parent = 0);
    ~MainWorker();

private slots:
    void on_inputFilePushButton_clicked();

    void on_transferPushButton_clicked();

    void on_refreshPushButton_clicked();

    void on_connectPushButton_clicked();

    void on_serialSendPushButton_clicked();

    void on_serialSendLineEdit_returnPressed();

    void on_serialClearPushButton_clicked();

    void commWindowAppend(QString text);

    void setSerialIsConnected(bool isConnected);

    void progressChange(int progress);


private:
    Ui::MainWorker *ui;
    QSerialPort *mSerialPort;
    QByteArray serialPortMessageBuffer;
    SerialWorker *mSerialWorker;
    QThread *serialThread;
    bool isSerialConnected, signalsConnected;
    int writeLimit;

    void refreshPorts();
};

#endif // MAINWORKER_H
