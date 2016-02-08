#include "mainworker.h"
#include "ui_mainworker.h"

MainWorker::MainWorker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWorker)
{
    ui->setupUi(this);
    writeLimit = 80;
    setProgress(0);
    signalsConnected = false;
    mSerialPort = new QSerialPort(this);

    refreshPorts();
}

void delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MainWorker::setProgress(int progress)
{
    if (ui->progressBar->value() != progress)
    {
        ui->progressBar->setValue(progress);
    }
}

MainWorker::~MainWorker()
{
    delete ui;
    setSerialIsConnected(false);
    writeLimit = 0.10;

    refreshPorts();
}

void MainWorker::on_inputFilePushButton_clicked()
{
    ui->inputFileLineEdit->setText(QFileDialog::getOpenFileName(this, "Open File...", "/"));

    if (!ui->inputFileLineEdit->text().isEmpty())
    {
        QFileInfo mFileInfo(ui->inputFileLineEdit->text());
        ui->ouputFileLineEdit->setText("/home/$USER/" + mFileInfo.baseName());
    }
}

void MainWorker::on_transferPushButton_clicked()
{
    if ((ui->inputFileLineEdit->text().isEmpty()) || (ui->ouputFileLineEdit->text().isEmpty()) || (!isSerialConnected))
    {
        commWindowAppend("ERROR!\n Please make sure input and output fields are completed and that you are connected to a serial device.\n");
        return;
    }
    QFile inputFile(ui->inputFileLineEdit->text());
    QString inputFileBytes;
    QString xified;
    int progress = 0;
    if (!(inputFile.open(QIODevice::ReadOnly)))
    {
        return;
    }
    inputFileBytes = inputFile.readAll().toHex();

    for (int filePos = 0; filePos < inputFileBytes.size(); filePos += 2)
    {
        xified.append("\\x" + inputFileBytes.mid(filePos, 2));
    }

    while( progress < xified.size() )
    {
        ui->progressBar->setValue((int)((double)(progress)/(double)(xified.size()) * 100));
        QString chunk = "echo -n -e \"";
        chunk.append(xified.mid(progress, writeLimit));
        chunk.append("\" >> " + ui->ouputFileLineEdit->text() + "\n");
        progress += writeLimit;
        mSerialPort->write(chunk.toLocal8Bit());
        delay(ui->delayDoubleSpinBox->value() * 1000);
    }

    ui->progressBar->setValue(100);
}


void MainWorker::on_refreshPushButton_clicked()
{
    refreshPorts();
}

void MainWorker::commWindowAppend(QString text)
{
    QString tempText = ui->commTextBrowser->toPlainText();
    tempText.append(text);
    ui->commTextBrowser->clear();
    ui->commTextBrowser->setPlainText(tempText);

    QScrollBar *bottomFeeder = ui->commTextBrowser->verticalScrollBar();
    bottomFeeder->setValue(bottomFeeder->maximum());
}

void MainWorker::on_serialClearPushButton_clicked()
{
    ui->commTextBrowser->clear();
}

void MainWorker::on_commTextBrowser_textChanged()
{

}


/********************  Serial Functions  *********************/

void MainWorker::refreshPorts()
{
    ui->portsComboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->portsComboBox->addItem(info.systemLocation());
}

void MainWorker::setSerialIsConnected(bool isConnected)
{
    //simplify UI changes by putting it in one function
    if(isConnected)
    {
        isSerialConnected = true;
        ui->connectPushButton->setText("Disconnect");
        commWindowAppend("\nSerial port connected!\n");
        ui->serialStatusLabel->setText("Connected");
        ui->serialPortLabel->setText(ui->portsComboBox->currentText());
        ui->portsComboBox->setEnabled(false);
        ui->refreshPushButton->setEnabled(false);
        ui->baudRateLineEdit->setEnabled(false);

        if (!signalsConnected)
        {
            connect(mSerialPort, SIGNAL(aboutToClose()), this, SLOT(onSerialPortDisconnect()));
            connect(mSerialPort, SIGNAL(readyRead()), this, SLOT(onSerialPortMessage()));
            signalsConnected = true;
        }
    }
    else
    {
        isSerialConnected = false;
        ui->connectPushButton->setText("Connect");
        commWindowAppend("\nSerial port disconnected\n");
        ui->serialStatusLabel->setText("Disconnected");
        ui->serialPortLabel->setText("");
        ui->portsComboBox->setEnabled(true);
        ui->refreshPushButton->setEnabled(true);
        ui->baudRateLineEdit->setEnabled(true);
    }
}

void MainWorker::on_connectPushButton_clicked()
{
    if (!isSerialConnected) //check if serial port is already connected
    {
        mSerialPort = new QSerialPort(this);
        mSerialPort->setPortName(ui->portsComboBox->currentText());
        mSerialPort->setBaudRate(ui->baudRateLineEdit->text().toInt());

        if (!mSerialPort->open(QIODevice::ReadWrite))
        {
            //if serial port connection fails
            commWindowAppend("\nFailed to open serial port...\n");
        }
        else
        {
            //serial port open succeeded
            setSerialIsConnected(true);
        }
    }
    else //if serial port already connected, assume user wanted to disconnect.
    {
        mSerialPort->close();
    }
}

void MainWorker::onSerialPortMessage()
{
    QByteArray serialBuffer;
    serialBuffer = mSerialPort->readAll();
    commWindowAppend(serialBuffer);
}

void MainWorker::onSerialPortDisconnect()
{
    setSerialIsConnected(false);
}

void MainWorker::on_serialSendPushButton_clicked()
{
    if (isSerialConnected)
    {
        if (ui->serialSendLineEdit->text().isEmpty())
        {
            mSerialPort->write(QString('\n').toLocal8Bit());
        }
        else
        {
            mSerialPort->write(QString(ui->serialSendLineEdit->text() + '\n').toLocal8Bit());
            ui->serialSendLineEdit->selectAll();
        }
    }
    else
    {
        commWindowAppend("\nError!\nNot connected to serial port...\n");
    }
}

void MainWorker::on_serialSendLineEdit_returnPressed()
{
    on_serialSendPushButton_clicked();
}

/*************** END SERIAL FUNCTIONS ********************/


