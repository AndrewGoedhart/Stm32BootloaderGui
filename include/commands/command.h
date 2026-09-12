#ifndef COMMAND_H
#define COMMAND_H

#include <memory>
#include <QObject>
#include <QList>
#include <QString>
#include <queue>

class CommandSequence;
class CommandStep;

using Bytes = std::vector<uint8_t>;
using RxStream = std::queue<uint8_t>;


class Command {
    using CmdPtr = std::shared_ptr<Command>;
    using StepPtr = std::shared_ptr<CommandStep>;

    QString _name;
    std::vector<StepPtr> _steps;
    CommandSequence *_owner;
    int _currentStep;
    int retries;
    int maxRetries;

    RxStream rxStream;

public:
    Command();
    Command(QString name);
    Command(QString name, uint32_t retriesAllowed);

    QString name();
    void setOwner(CommandSequence *owner);
    void addStep(StepPtr testStep);

    virtual void execute();
    virtual void rxData( const QByteArray &data);
    virtual void timerExpired();

    // Services for steps
    void txData(const Bytes &data);
    void scheduleTimeout(int milliseconds);


    // notifications from steps
    void stepComplete();
    virtual void stepFailed();

    // factory methods
    static CmdPtr resetIntoCustomBootLoader(CommandSequence *seq);
    static CmdPtr resetIntoChipBootLoader(CommandSequence *seq);
    static CmdPtr resetIntoRunMode(CommandSequence *seq);
    static CmdPtr syncBaud();
    static CmdPtr enableDebug(CommandSequence *seq);
    static CmdPtr bootloadSlave(CommandSequence *seq);
    static CmdPtr eraseFlash();
    static CmdPtr writeData(uint32_t address, const Bytes &dataPage);
    static CmdPtr verifyData(uint32_t address,const Bytes &dataPage);
    static CmdPtr programCrc();
    static CmdPtr verifyCrc(uint32_t address, uint32_t size, uint32_t expectedCrc);

    void executeNextStep();

    static Bytes toBytes(uint32_t address);
};

#endif // COMMAND_H
