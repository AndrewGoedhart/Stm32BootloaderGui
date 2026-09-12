#ifndef COMMANDSEQUENCE_H
#define COMMANDSEQUENCE_H
#include <QObject>
#include <QString>
#include <QTimer>
#include <commands/command.h>
#include <softwareimage.h>
#include <SerialSource.h>


class CommandSequence :public QObject {
    Q_OBJECT

 public:
    explicit CommandSequence(SerialSource &source, QObject *parent = nullptr);
    virtual ~CommandSequence()=default;

    void reset();
    void addCommand(const std::shared_ptr<Command> &cmd);

    // generate events for commands
    void start();
    void rxData(const QByteArray &data);
    void timerExpired();
    void executeCurrentCmd();

public slots:
      void CommandCompletedOkay();
      void CommandFailed();
      void txData(const Bytes &packet);
      void scheduleTimeout(int milliseconds);
      void stopTimer();
      void resetSerialPort();
      void clearQueues();
      void dtr(bool isEnabled);
      void rts(bool isEnabled);

    signals:
      void updateProgress(QString command, int percent);
      void downloadComplete();
      void downloadFailed();



    private:
       SerialSource *_serialPort;
       QTimer  _timer;

       QList<std::shared_ptr<Command>> _commands;
       int _currentCommandIndex;
       bool _active;

};


#endif // COMMANDSEQUENCE_H
