#include <commands/commandstep.h>


void CommandStep::setCommand(Command *owner){
  _owner = owner;
}

Command *CommandStep::cmd() {return _owner;}

CommandStep::StepPtr CommandStep::txBaudSync()
{
   auto exec = [](Command *cmd){
       cmd->txData(createArray({0x7F}));
   };
   auto rxData = [](Command *cmd, const QByteArray &data){
       if( data.size() > 0){
         if( static_cast<uint8_t>(data[0]) == ACK ){
           cmd->stepComplete();
         }
       }
     };
   auto timerExpired =[](Command *cmd){
       cmd->stepFailed();
    };
    return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData, timerExpired);
}

CommandStep::StepPtr CommandStep::txCmd(uint8_t cmdVal){
  auto exec = [cmdVal](Command *cmd){
      cmd->txData(createArray({cmdVal, static_cast<uint8_t>(~cmdVal)}));
  };

  auto rxData = [](Command *cmd, const QByteArray &data){
      if( data.size() > 0){
        if( static_cast<uint8_t>(data[0]) == ACK ){
          cmd->stepComplete();
        }
        if( static_cast<uint8_t>(data[0]) == NACK ){
          cmd->stepFailed();
        }
      }
    };
    auto timerExpired =[](Command *cmd){
        cmd->stepFailed();
     };
     return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData, timerExpired);

}

QByteArray CommandStep::appendCheckSum(const QByteArray &data)
{
    QByteArray packet(data);
    uint8_t checksum = 0;
    for(auto d:data ){
      checksum = static_cast<uint8_t>(checksum ^d);
    }
    packet.append(checksum);
    return packet;
}

CommandStep::StepPtr CommandStep::txFixedData(const QByteArray &data){
  QByteArray packet =appendCheckSum(data);
  auto exec = [packet](Command *cmd){
      cmd->txData(packet);
  };

  auto rxData = [](Command *cmd, const QByteArray &data){
      if( data.size() > 0){
        if( static_cast<uint8_t>(data[0]) == ACK ){
          cmd->stepComplete();
        }
        if( static_cast<uint8_t>(data[0]) == NACK ){
          cmd->stepFailed();
        }
      }
    };
    auto timerExpired =[](Command *cmd){
        cmd->stepFailed();
     };
     return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData, timerExpired);

}

CommandStep::StepPtr CommandStep::txVariableData(const QByteArray &data){
    QByteArray packet;
    packet.append(data.size()-1);
    packet.append(data);
    return txFixedData(packet);
}



QByteArray CommandStep::createArray(std::initializer_list<uint8_t> data){
  auto result = QByteArray();
  result.reserve(data.size());
  for( auto e:data){
    result.append(e);
  }
  return result;
}

void RxVerifyData::execute(){
  uint8_t len = static_cast<uint8_t>(_expected.size()-2);
  QByteArray packet;
  packet.append(len);
  packet.append(~len);
  _owner->txData(packet);

}

void RxVerifyData::rxData(const QByteArray &data) {
    for(auto c :data){
        if( c != _expected[_rxIndex]){
            if( _owner != nullptr){
                _owner->stepFailed();
            }
        }
        _rxIndex++;
        if( _rxIndex == _expected.size()){
            _owner->stepComplete();
        }
    }
}
