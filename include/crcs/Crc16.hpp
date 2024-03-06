#ifndef COMMON_CRCS_INCLUDE_CRCS_CRC16_HPP
#define COMMON_CRCS_INCLUDE_CRCS_CRC16_HPP

#include <cstdint>
#include <crcs/Crc.hpp>

namespace Crcs {
  using Crc16Ibm = Crc<uint16_t, 0x1021, 16, 0xFFFF, 0x0000 >;
  using Crc16Kermit = ReflectedCrc<uint16_t, 0x8408, 16, 0x0000, 0x0000 >;
}

#endif
