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

    void onSerialPortDisconnect();

    void onSerialPortMessage();

    void on_serialSendPushButton_clicked();

    void on_serialSendLineEdit_returnPressed();

    void on_serialClearPushButton_clicked();

    void on_commTextBrowser_textChanged();

private:
    Ui::MainWorker *ui;
    QSerialPort *mSerialPort;
    QByteArray serialPortMessageBuffer;
    bool isSerialConnected, signalsConnected;
    int writeLimit;

    void refreshPorts();
    void setSerialIsConnected(bool isConnected);
    void setProgress(int progress);
    void commWindowAppend(QString text);

};

#endif // MAINWORKER_H
