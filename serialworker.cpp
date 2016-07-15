#include "serialworker.h"

SerialWorker::SerialWorker(QObject *parent)
    : QObject(parent),
      _serialPort(new QSerialPort()),
      _serialBuffer(QString()),
      _isConnected(false),
      _isTransferring(false)
{
    connect(_serialPort, SIGNAL(aboutToClose()), this, SLOT(_aboutToClose()));
    connect(_serialPort, SIGNAL(readyRead()), this, SLOT(_readyRead()));
}

SerialWorker::~SerialWorker()
{
    _serialPort->close();
    _serialPort->deleteLater();
}

void SerialWorker::ConnectToSerialPort(QString PortName, int BaudRate)
{
    if (_isConnected)
    {
        emit StatusMessage("Serial port already connected.");
        return;
    }

    _serialPort->setPortName(PortName);
    _serialPort->setBaudRate(BaudRate);

    if (!_serialPort->open(QIODevice::ReadWrite))
    {
        emit StatusMessage("Error connecting to serial port.");
        return;
    }

    _isConnected = true;
    emit StatusMessage("Serial Port Connected; Port: " + PortName + " Baud: " + QString::number(BaudRate));
    emit SerialPortConnected();
}

void SerialWorker::DisconnectSerialPort()
{
    _serialPort->close();
}

void SerialWorker::SendSerialMessage(QString Message)
{
    if (!_isConnected && !_isTransferring)
    {
        emit StatusMessage("Error sending message, no Serial Port connected.");
        return;
    }
    _serialPort->write(Message.toLocal8Bit());
}

void SerialWorker::_aboutToClose()
{
    _isConnected = false;
    emit AboutToClose();
}

void SerialWorker::_readyRead()
{
    if (!_isTransferring)
    {
        QByteArray serialBytes = _serialPort->readAll();

        for (int indexCounter = 0; indexCounter < serialBytes.size(); indexCounter++)
        {
            if (serialBytes[indexCounter] == '\n')
            {
                emit NewSerialMessage(_serialBuffer);
                _serialBuffer.clear();
            }
            else if (serialBytes[indexCounter] == '\r')
            {
            }
            else
            {
                _serialBuffer.append(QString(serialBytes.at(indexCounter)));
            }
        }
    }
}

void SerialWorker::DelayTime(int MillisecondsToWait)
{
    QTime dieTime = QTime::currentTime().addMSecs( MillisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void SerialWorker::TransferFile(QString InputFile, QString OutputFile, int ChunkSize, int Delay)
{
    emit StatusMessage("Opening File...");
    QFile inputFile(InputFile);
    QString inputFileBytes;
    QString xified;
    if (!(inputFile.open(QIODevice::ReadOnly)))
    {
        emit StatusMessage("Error sending message, no Serial Port connected.");
        return;
    }
    emit StatusMessage("Parsing File...");
    inputFileBytes = inputFile.readAll().toHex();
    inputFile.close();
    for (int filePos = 0; filePos < inputFileBytes.size(); filePos += 2)
    {
        xified.append("\\x" + inputFileBytes.mid(filePos, 2));
    }

    emit StatusMessage("Transferring File...");
    _isTransferring = true;
    for (int progress = 0; progress < xified.size(); progress += ChunkSize)
    {
        QString chunk = "echo -n -e \"";
        chunk.append(xified.mid(progress, ChunkSize));
        chunk.append("\" >> " + OutputFile + "\n");
        _serialPort->write(chunk.toLocal8Bit());

        emit ProgressChange(((int)((double)(progress)/(double)(xified.size()) * 100)));
        DelayTime(Delay);
    }
    _isTransferring = false;
    emit StatusMessage("File Transfer Complete.");
    emit FinishedFileTransfer();
    emit ProgressChange(100);
}

