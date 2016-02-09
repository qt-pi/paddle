#include "mainworker.h"
#include "ui_mainworker.h"

MainWorker::MainWorker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWorker)
{
    ui->setupUi(this);
    progressChange(0);
    writeLimit = 60;
    setSerialIsConnected(false);

    serialThread = new QThread(this);
    mSerialWorker = new SerialWorker();
    mSerialWorker->moveToThread(serialThread);

    connect(serialThread, SIGNAL(started()), mSerialWorker, SLOT(startConnect()));
    connect(serialThread, SIGNAL(finished()), mSerialWorker, SLOT(closeConnection()));
    connect(mSerialWorker, SIGNAL(newSerialText(QString)), this, SLOT(commWindowAppend(QString)));
    connect(mSerialWorker, SIGNAL(serialConnectionChange(bool)), this, SLOT(setSerialIsConnected(bool)));
    connect(mSerialWorker, SIGNAL(progressChange(int)), this, SLOT(progressChange(int)));

    refreshPorts();
}

void MainWorker::progressChange(int progress)
{
    if (ui->progressBar->value() != progress)
    {
        ui->progressBar->setValue(progress);
    }
}

MainWorker::~MainWorker()
{

    delete ui;
    mSerialWorker->closeConnection();
    mSerialWorker->deleteLater();
    serialThread->deleteLater();
}

void MainWorker::on_inputFilePushButton_clicked()
{
    ui->inputFileLineEdit->setText(QFileDialog::getOpenFileName(this, "Open File...", "/"));

    if (!ui->inputFileLineEdit->text().isEmpty())
    {
        QFileInfo mFileInfo(ui->inputFileLineEdit->text());
        ui->ouputFileLineEdit->setText(mFileInfo.fileName());
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
    if (!(inputFile.open(QIODevice::ReadOnly)))
    {
        return;
    }
    inputFileBytes = inputFile.readAll().toHex();

    for (int filePos = 0; filePos < inputFileBytes.size(); filePos += 2)
    {
        xified.append("\\x" + inputFileBytes.mid(filePos, 2));
    }

    mSerialWorker->setFileTransferParams(xified, ui->ouputFileLineEdit->text(), writeLimit, ui->delayDoubleSpinBox->value() * 1000);
    mSerialWorker->startTransfer();
}


void MainWorker::on_refreshPushButton_clicked()
{
    refreshPorts();
}

void MainWorker::commWindowAppend(QString text)
{
    QString tempText = ui->commTextBrowser->toPlainText();
    if (tempText.size() > 2048)
        tempText.clear();
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

void MainWorker::refreshPorts()
{
    ui->portsComboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->portsComboBox->addItem(info.systemLocation());
}

void MainWorker::setSerialIsConnected(bool isConnected)
{
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
    if (!isSerialConnected)
    {
      mSerialWorker->setSerialParams(ui->portsComboBox->currentText(), ui->baudRateLineEdit->text().toInt());
      serialThread->start();
    }
    else
    {
        mSerialWorker->closeConnection();
    }
}

void MainWorker::on_serialSendPushButton_clicked()
{
    if (isSerialConnected)
    {
        mSerialWorker->sendSerialMessage(QString(ui->serialSendLineEdit->text() + "\n"));
        ui->serialSendLineEdit->selectAll();
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


