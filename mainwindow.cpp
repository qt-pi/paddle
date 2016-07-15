#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _isConnected(false)
{
    ui->setupUi(this);

    _thread = new QThread();
    _serialWorker = new SerialWorker();
    _serialWorker->moveToThread(_thread);
    connect(_serialWorker, SIGNAL(AboutToClose()), this, SLOT(SerialPortDisconnected()));
    connect(_serialWorker, SIGNAL(SerialPortConnected()), this, SLOT(SerialPortConnected()));
    connect(_serialWorker, SIGNAL(NewSerialMessage(QString)), this, SLOT(NewSerialMessage(QString)));
    connect(_serialWorker, SIGNAL(StatusMessage(QString)), this, SLOT(SetStatusBarMessage(QString)));
    connect(_serialWorker, SIGNAL(ProgressChange(int)), this, SLOT(ProgressChange(int)));
    connect(_serialWorker, SIGNAL(FinishedFileTransfer()), this, SLOT(FinishedFileTransfer()));
    _thread->start();
    ui->progressBar->setValue(0);
    SerialConnectedUI(false);
    RefreshPorts();
}

MainWindow::~MainWindow()
{
    delete ui;
    _serialWorker->deleteLater();
    _thread->quit();
    _thread->deleteLater();
}

void MainWindow::RefreshPorts()
{
    ui->_portsComboBox->clear();
    foreach(const QSerialPortInfo &port, QSerialPortInfo::availablePorts())
        ui->_portsComboBox->addItem(port.systemLocation());
}

void MainWindow::on__refreshPushButton_clicked()
{
    RefreshPorts();
}

void MainWindow::SerialPortConnected()
{
    ui->_textBrowser->append("Connected Succesfully!");
    ui->_portsComboBox->setEnabled(false);
    ui->_baudRateLineEdit->setEnabled(false);
    ui->_refreshPushButton->setEnabled(false);
    ui->_connectPushButton->setText("Disconnect");

    SerialConnectedUI(true);
}

void MainWindow::SerialPortDisconnected()
{
    ui->_textBrowser->append("Serial port disconnected");
    ui->_portsComboBox->setEnabled(true);
    ui->_baudRateLineEdit->setEnabled(true);
    ui->_refreshPushButton->setEnabled(true);
    ui->_connectPushButton->setText("Connected");

    SerialConnectedUI(false);
}

void MainWindow::NewSerialMessage(QString Message)
{
    ui->_textBrowser->append(Message);
}

void MainWindow::SetStatusBarMessage(QString Message)
{
    ui->statusBar->showMessage(Message);
}

void MainWindow::SerialConnectedUI(bool SetEnable)
{
    ui->_selectFilePushButton->setEnabled(SetEnable);
    ui->_outputFileLineEdit->setEnabled(SetEnable);
    ui->_chunkSizeSpinBox->setEnabled(SetEnable);
    ui->_packetDelaySpinBox->setEnabled(SetEnable);
    ui->_transferPushButton->setEnabled(SetEnable);
    ui->_commandLineEdit->setEnabled(SetEnable);
    ui->_sendPushButton->setEnabled(SetEnable);
    ui->_md5CheckBox->setEnabled(SetEnable);

    _isConnected = SetEnable;
}

void MainWindow::on__connectPushButton_clicked()
{
    if ((ui->_portsComboBox->currentText().isEmpty()) || (ui->_baudRateLineEdit->text().isEmpty()))
    {
        ui->_textBrowser->append("Error: No port selected or no baud rate specified.");
        return;
    }
    if (!_isConnected)
    {
        _serialWorker->ConnectToSerialPort(ui->_portsComboBox->currentText(), ui->_baudRateLineEdit->text().toInt());
    }
    else
    {
        _serialWorker->DisconnectSerialPort();
    }
}

void MainWindow::on__sendPushButton_clicked()
{
    if (!ui->_commandLineEdit->text().isEmpty())
    {
        _serialWorker->SendSerialMessage(QString(ui->_commandLineEdit->text() + "\n"));
    }
    else
    {
        _serialWorker->SendSerialMessage("\n");
    }
}

void MainWindow::on__commandLineEdit_returnPressed()
{
    on__sendPushButton_clicked();
    ui->_commandLineEdit->selectAll();
}

void MainWindow::ProgressChange(int NewProgress)
{
    if (ui->progressBar->value() != NewProgress)
    {
        ui->progressBar->setValue(NewProgress);
    }
}

void MainWindow::on__transferPushButton_clicked()
{
    if (ui->_inputFileLabel->text().isEmpty() || ui->_outputFileLineEdit->text().isEmpty())
    {
        ui->statusBar->showMessage("Please fill out input and output file fields.");
        return;
    }
    _serialWorker->TransferFile(ui->_inputFileLabel->text(), ui->_outputFileLineEdit->text(), ui->_chunkSizeSpinBox->value(), (int)(ui->_packetDelaySpinBox->value() * 1000));
}

void MainWindow::on__selectFilePushButton_clicked()
{
    ui->_inputFileLabel->setText(QFileDialog::getOpenFileName(this, "Open File...", "/"));
    if (!ui->_inputFileLabel->text().isEmpty())
    {
        QCryptographicHash crypto(QCryptographicHash::Md5);
        QFile mFile(ui->_inputFileLabel->text());
        mFile.open(QIODevice::ReadOnly);
        QByteArray inputFileBytes = mFile.readAll();
        mFile.close();
        crypto.addData(inputFileBytes);
        ui->_md5sumLabel->setText(QString(crypto.result().toHex()));
    }
}

void MainWindow::FinishedFileTransfer()
{
    if (ui->_md5CheckBox->isChecked())
    {
        _serialWorker->SendSerialMessage(QString("md5sum " + ui->_outputFileLineEdit->text() + "\n"));
    }
}
