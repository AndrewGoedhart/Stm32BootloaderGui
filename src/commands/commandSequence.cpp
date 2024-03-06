#include "commands/commandSequence.h"

CommandSequence::CommandSequence(SerialSource &source, QObject *parent):
    QObject(parent),
    _serialPort(&source),
    _timer(this),
    _commands(),
    _active(false){
  connect(&_timer, &QTimer::timeout, this, &CommandSequence::timerExpired);
}

void CommandSequence::reset(){
  _commands.clear();
  _currentCommandIndex=0;
}

void CommandSequence::addCommand(const std::shared_ptr<Command> &cmd){
    _commands.append(cmd);
    cmd->setOwner(this);
}

void CommandSequence::start(){
   _currentCommandIndex = 0;
   executeCurrentCmd();
}

void CommandSequence::executeCurrentCmd()
{
    if(_currentCommandIndex < _commands.size()){
        updateProgress(_commands[_currentCommandIndex]->name(), _currentCommandIndex*100/_commands.size());
        _serialPort->clearQueues();
      _commands[_currentCommandIndex]->execute();
    } else {
      downloadComplete();
    }
}

void CommandSequence::CommandCompletedOkay(){
    _currentCommandIndex++;
    executeCurrentCmd();
}

void CommandSequence::CommandFailed(){
    _currentCommandIndex =0;
     downloadFailed();
}

void CommandSequence::txData(const Bytes &packet){
   if( _serialPort->isOpen()){
    _serialPort->txData(packet);
   }
}


void CommandSequence::scheduleTimeout(int milliseconds){
   _timer.setInterval(milliseconds);
   _timer.start();
}


void CommandSequence::stopTimer(){
   _timer.stop();
}

void CommandSequence::timerExpired(){
  if( _currentCommandIndex < _commands.size()) {
    _commands[_currentCommandIndex]->timerExpired();
  }
}

void CommandSequence::rxData(const QByteArray &data){
  if( _currentCommandIndex < _commands.size()) {
    _commands[_currentCommandIndex]->rxData(data);
  }

}

void CommandSequence::dtr(bool isEnabled){
    _serialPort->dtr(isEnabled);
}
void CommandSequence::rts(bool isEnabled){
    _serialPort->rts(isEnabled);
}

void CommandSequence::clearQueues() {
  _serialPort->clearQueues();
}

void CommandSequence::resetSerialPort() {
  auto port = _serialPort->getPort();
  auto baud = _serialPort->getBaudRate();
  _serialPort->closePort();
  _serialPort->openPort(port, baud);
}
