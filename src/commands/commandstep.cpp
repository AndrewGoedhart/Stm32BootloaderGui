#include <commands/commandstep.h>

using RxStream = std::queue<uint8_t>;


uint8_t getNext(RxStream &stream) {
  uint8_t byte = stream.front();
  stream.pop();
  return byte;
}


void CommandStep::setCommand(Command *owner) {
  _owner = owner;
}

Command *CommandStep::cmd() { return _owner; }

CommandStep::StepPtr CommandStep::txBaudSync() {
  static auto txCount = 0;
  auto exec = [](Command *cmd) {
    txCount = 0;
    cmd->txData(createArray({0x7F}));
    cmd->scheduleTimeout(500);
  };

  auto rxData = [](Command *cmd, RxStream &data) {
    if (data.size() > 0) {
      auto byte = getNext(data);
      if (byte == ACK) {
        cmd->stepComplete();
      } else {
        cmd->stepFailed();
      }
    }
  };

  auto timerExpired = [](Command *cmd) {
    txCount++;
    if (txCount > 10) {
      cmd->stepFailed();
    } else {
      cmd->txData(createArray({0x7F}));
      cmd->scheduleTimeout(500);
    }
  };
  return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData,
                                                                                                timerExpired);
}

CommandStep::StepPtr CommandStep::txCmd(uint8_t cmdVal) {
  auto exec = [cmdVal](Command *cmd) {
    cmd->txData(createArray({cmdVal, static_cast<uint8_t>(~cmdVal)}));
    cmd->scheduleTimeout(10000);
  };

  auto rxData = [](Command *cmd, RxStream &data) {
    if (data.size() > 0) {
      auto rxByte = getNext(data);
      if (rxByte == ACK) {
        cmd->stepComplete();
      } else if (rxByte == NACK) {
        cmd->stepFailed();
      }
    }
  };
  auto timerExpired = [](Command *cmd) {
    cmd->stepFailed();
  };
  return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData,
                                                                                                timerExpired);

}

Bytes CommandStep::appendCheckSum(const Bytes &data) {
  Bytes packet(data);
  uint8_t checksum = 0;
  for (auto d: data) {
    checksum = static_cast<uint8_t>(checksum ^ d);
  }
  packet.push_back(checksum);
  return packet;
}

CommandStep::StepPtr CommandStep::txFixedData(const Bytes &data) {
  Bytes packet = appendCheckSum(data);
  auto exec = [packet](Command *cmd) {
    cmd->scheduleTimeout(10000);
    cmd->txData(packet);
  };

  auto rxData = [](Command *cmd, RxStream &data) {
    if (data.size() > 0) {
      auto byte = getNext(data);
      if (byte == ACK) {
        cmd->stepComplete();
      }
      if (byte == NACK) {
        cmd->stepFailed();
      }
    }
  };
  auto timerExpired = [](Command *cmd) {
    cmd->stepFailed();
  };
  return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData,
                                                                                                timerExpired);

}

CommandStep::StepPtr CommandStep::rxCrc(uint32_t expectedCrc) {

  auto exec = [](Command *cmd) {
    cmd->scheduleTimeout(10000);
  };


  auto rxData = [expectedCrc](Command *cmd, RxStream &data) {
    if (data.size() >= 6) {
      auto ack = getNext(data);
      if (ack == ACK) {
        uint32_t crc = getNext(data);
        crc |= getNext(data) << 8;
        crc |= getNext(data) << 16;
        crc |= getNext(data) << 24;
        if (crc == expectedCrc) {
          cmd->stepComplete();
        } else {
          cmd->stepFailed();
        }
      }
      if (ack == NACK) {
        cmd->stepFailed();
      }
    }
  };

  auto timerExpired = [](Command *cmd) {
    cmd->stepFailed();
  };
  return std::make_shared<LambdaStep<decltype(exec), decltype(rxData), decltype(timerExpired)>>(exec, rxData,
                                                                                                timerExpired);

}


CommandStep::StepPtr CommandStep::txVariableData(const Bytes &data) {
  std::vector<uint8_t> packet;
  packet.push_back(static_cast<uint8_t>(data.size() - 1));
  packet.insert(packet.end(), data.begin(), data.end());
  return txFixedData(packet);
}


std::vector<uint8_t> CommandStep::createArray(std::initializer_list<uint8_t> data) {
  auto result = std::vector<uint8_t>();
  result.insert(result.end(), data.begin(), data.end());
  return result;
}

void RxVerifyData::execute() {
  auto len = static_cast<uint8_t>(_expected.size() - 2);
  auto packet = Bytes();
  packet.push_back(len);
  packet.push_back(~len);
  _owner->txData(packet);

}

void RxVerifyData::rxData(RxStream &data) {
  if (data.size() >= _expected.size()) {
    for (auto e: _expected) {
      auto c = getNext(data);
      if (c != e) {
        _owner->stepFailed();
        return;
      }
    }
    _owner->stepComplete();
  }
}
