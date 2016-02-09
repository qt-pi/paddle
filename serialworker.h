#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QCoreApplication>
#include <QSerialPort>
#include <QTime>
#include <QDebug>

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = 0);

    void setPortName(QString portName);

    void setBaudRate(int baudRate);

    void sendSerialMessage(QString message);

    void setSerialParams(QString PortName, int BaudRate);

    void setFileTransferParams(QString fileData, QString OutputFile, int WriteLimit, int DelayTime);

signals:
    void newSerialText(QString text);

    void serialConnectionChange(bool isConnected);

    void progressChange(int progress);

private slots:
    void onSerialPortDisconnect();

    void onSerialPortMessage();

public slots:

    void startConnect();

    void startTransfer();

    void closeConnection();

private:
    QSerialPort *mSerialPort;

    QString mFileData;

    QString outputFile;

    int writeLimit, delayTime;

    bool isSerialConnected;

    void delay(int millisecondsToWait);

    void setSerialConnectionState(bool connectionState);

};

#endif // SERIALWORKER_H
