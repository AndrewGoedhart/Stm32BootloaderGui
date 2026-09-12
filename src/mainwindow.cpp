#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QtSerialPort>
#include <SerialSource.h>
#include "SerialSetup.h"
#include <softwareimage.h>
#include <commands/command.h>


template <typename FN1, typename FN2> void MainWindow::download(QLineEdit *srcFile, uint32_t flashStart, FN1 preSequence, FN2 postSequence){
  SoftwareImage image(PageSize);
  if( image.openFile(srcFile->text())){
      setupSerial();
      if( _serial->isOpen()){
          enableButtons(false);
          _softwareCmds->reset();
          preSequence(_softwareCmds);
          createDownloadSequence(_softwareCmds, &image, flashStart);
          postSequence(_softwareCmds, image);
          _softwareCmds->start();
      }
  } else {
      ui->logView->logLine(" Opening image file failed!");
  }
}





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      _serial(new SerialSource(this)),
       _actionLabel(""),
      _softwareCmds(new CommandSequence(*_serial, this)){
    ui->setupUi(this);
    ui->pbDownload->setValue(0);
    ui->cbBaudRate->addItems(SerialSetup::availableBaudRates());
    ui->cbBaudRate->setCurrentText("115200");
    ui->cbDevice->addItems(SerialSetup::availablePorts());
    ui->cbDevice->setCurrentText("/dev/ttyUSB0");
    ui->btnCancel->setEnabled(false);
    connect(_serial, &SerialSource::rxData, _softwareCmds, &CommandSequence::rxData);
    connect(_softwareCmds, &CommandSequence::updateProgress, this, &MainWindow::updateProgress);
    connect(_softwareCmds, &CommandSequence::downloadComplete, this, &MainWindow::downloadComplete);
    connect(_softwareCmds, &CommandSequence::downloadFailed, this, &MainWindow::downloadFailed);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::updateProgress(QString command, int percent){
    ui->logView->logLine(command);
    ui->pbDownload->setValue(percent);
}

void MainWindow::setupSerial()
{
    _serial->closePort();
    auto port = ui->cbDevice->currentText();
    auto baud = ui->cbBaudRate->currentText().toInt();
    _serial->openPort(port, baud);
}

void MainWindow::loadFile(QString Prompt, QLineEdit *leFile){
    QString binFile = QFileDialog::getOpenFileName(
       this, tr(""), "./", tr("Bin Files(*.bin)"));
       leFile->setText(binFile);
}

void MainWindow::createDownloadSequence(CommandSequence *seq, SoftwareImage *image, uint32_t startAddress){
  _actionLabel= "Download";
  seq->addCommand(Command::syncBaud());
  seq->addCommand(Command::eraseFlash());
  for( uint32_t addr = 0; addr< image->getImageSize(); addr+= PageSize){
      seq->addCommand(Command::writeData(addr+startAddress, image->getDataSegment(addr, PageSize)));
  }
  for( uint32_t addr = 0; addr< image->getImageSize(); addr+= PageSize){
      seq->addCommand(Command::verifyData(addr+startAddress, image->getDataSegment(addr, PageSize)));
  }
}



void MainWindow::downloadFailed(){
    ui->logView->logLine(" ## "+_actionLabel+" Failed ##");
    enableButtons(true);
}

void MainWindow::downloadComplete(){
    ui->logView->logLine(" ## "+_actionLabel +" Complete ##");
    enableButtons(true);
}

void MainWindow::on_btnDebug_clicked()
{
    setupSerial();
    if( _serial->isOpen()){
        enableButtons(false);
      _actionLabel = "Enable Debug";
      _softwareCmds->reset();
      _softwareCmds->addCommand(Command::resetIntoCustomBootLoader(_softwareCmds));
      _softwareCmds->addCommand(Command::syncBaud());
      _softwareCmds->addCommand(Command::enableDebug(_softwareCmds));
      _softwareCmds->start();
    }
}

void MainWindow::on_btnReset_clicked(){
    enableButtons(false);
    setupSerial();
    if( _serial->isOpen()){
    _actionLabel = "Reset into User code";
    _softwareCmds->reset();
    _softwareCmds->addCommand(Command::resetIntoRunMode(_softwareCmds));
    _softwareCmds->start();
    }
}

void MainWindow::enableButtons(bool enableDownload){
    ui->btnBootloader->setEnabled(enableDownload);
    ui->btnDC->setEnabled(enableDownload);
    ui->btnApp->setEnabled(enableDownload);
    ui->btnCancel->setEnabled(!enableDownload);
}

void MainWindow::on_tbLoadBoot_clicked()
{
    loadFile("Load software boot binary", ui->leBootFile);
}


void MainWindow::on_tbLoadMain_clicked()
{
    loadFile("Load software Main binary", ui->leSoftwareFile);
}

void MainWindow::on_tbLoadDc_clicked()
{
    loadFile("Load software DC binary", ui->leDcFile);
}

// @brief download the selected bootloader.
void MainWindow::on_btnBootloader_clicked()
{
    download(ui->leBootFile, FlashBootStart,
                [](auto &seq){  seq->addCommand(Command::resetIntoChipBootLoader(seq));
                },
                [](auto &seq, auto &image){});
}

void MainWindow::on_btnDC_clicked()
{
    download(ui->leDcFile, FlashBootStart,
                [](auto &seq){
                    seq->addCommand(Command::resetIntoCustomBootLoader(seq));
                    seq->addCommand(Command::syncBaud());
                    seq->addCommand(Command::bootloadSlave(seq));
                },
                [](auto &seq, auto &image){});
}

void MainWindow::on_btnApp_clicked()
{
    download(ui->leSoftwareFile, FlashAppStart,
                [](auto &seq){
                  seq->addCommand(Command::resetIntoCustomBootLoader(seq));
                },
                [](auto &seq, auto &image){
                   seq->addCommand(Command::programCrc());
                   seq->addCommand(Command::verifyCrc(FlashAppStart, image.getImageSize(), image.getCrc(image.getImageSize()) ));
                });
}

