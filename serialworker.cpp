#include "serialworker.h"

SerialWorker::SerialWorker(QObject *parent) : QObject(parent)
{
    isSerialConnected = false;
    mFileData = "";
    outputFile = "";
    writeLimit = 80;
    delayTime = 100;

    mSerialPort = new QSerialPort(this);
    connect(mSerialPort, SIGNAL(readyRead()), this, SLOT(onSerialPortMessage()));
    connect(mSerialPort, SIGNAL(aboutToClose()), this, SLOT(onSerialPortDisconnect()));
}

void SerialWorker::delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void SerialWorker::setSerialParams(QString PortName, int BaudRate)
{

    mSerialPort->setPortName(PortName);
    mSerialPort->setBaudRate(BaudRate);
}

void SerialWorker::startConnect()
{

    if (!mSerialPort->open(QIODevice::ReadWrite))
    {
        setSerialConnectionState(false);
    }
    else
    {
        setSerialConnectionState(true);
    }
}

void SerialWorker::closeConnection()
{
    mSerialPort->close();
}

void SerialWorker::onSerialPortDisconnect()
{
    setSerialConnectionState(false);
}

void SerialWorker::setSerialConnectionState(bool connectionState)
{
    if (isSerialConnected != connectionState)
    {
        isSerialConnected = connectionState;
        emit serialConnectionChange(isSerialConnected);
    }
}

void SerialWorker::sendSerialMessage(QString message)
{
    if (isSerialConnected)
    {
        mSerialPort->write(message.toLocal8Bit());
    }
}

void SerialWorker::onSerialPortMessage()
{
    QString messageBuffer = mSerialPort->readAll();
    emit newSerialText(messageBuffer);
}

void SerialWorker::setFileTransferParams(QString fileData, QString OutputFile, int WriteLimit, int DelayTime)
{
    mFileData = fileData;
    outputFile = OutputFile;
    writeLimit = WriteLimit;
    delayTime = DelayTime;
}

void SerialWorker::startTransfer()
{

    for (int progress = 0; progress < mFileData.size(); progress += writeLimit)
    {
        QString chunk = "echo -n -e \"";
        chunk.append(mFileData.mid(progress, writeLimit));
        chunk.append("\" >> " + outputFile + "\n");
        mSerialPort->write(chunk.toLocal8Bit());

        emit progressChange(((int)((double)(progress)/(double)(mFileData.size()) * 100)));
        delay(delayTime);
    }

    emit progressChange(100);
}
