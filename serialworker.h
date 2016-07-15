#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QFile>
#include <QCoreApplication>
#include <QTime>
#include <QCryptographicHash>

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = 0);
    ~SerialWorker();
    void ConnectToSerialPort(QString PortName, int BaudRate);
    void DisconnectSerialPort();
    void SendSerialMessage(QString Message);
    void TransferFile(QString InputFile, QString OutputFile, int ChunkSize, int Delay);

signals:
    void ErrorMessage();
    void SerialPortConnected();
    void AboutToClose();
    void NewSerialMessage(QString);
    void StatusMessage(QString);
    void ProgressChange(int);
    void MD5Sum(QString);
    void FinishedFileTransfer();

public slots:

private slots:
    void _aboutToClose();
    void _readyRead();

private:
    void DelayTime(int MillisecondsToWait);
    QSerialPort* _serialPort;
    QString _serialBuffer;
    bool _isConnected;
    bool _isTransferring;
};

#endif // SERIALWORKER_H
