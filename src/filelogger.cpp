#include <filelogger.h>


FileLogger::FileLogger():
  _file{},
  _outputStream{}
{
}

void FileLogger::createLogFile(QString filename){
  _file.setFileName(filename);
  if( _file.open(QIODevice::WriteOnly | QIODevice::Text)){
    _outputStream = std::make_unique<QTextStream>(&_file);
  }
}

void FileLogger::closeLogFile(){
  if( _file.isOpen()){
    _file.flush();
    _file.close();
  }
}

bool FileLogger::isLogging()
{
  return _file.isOpen();
}

void FileLogger::logLine(QString line)
{
  if( isLogging()){
    *_outputStream << line<< Qt::endl;
  }
}

void FileLogger::log(QString line)
{
  if( isLogging()){
    *_outputStream << line;
  }
}
