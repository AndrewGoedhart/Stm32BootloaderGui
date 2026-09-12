#ifndef COMMON_CRCS_INCLUDE_CRCS_CRC8_HPP
#define COMMON_CRCS_INCLUDE_CRCS_CRC8_HPP

#include <cstdint>
#include <crcs/Crc.hpp>

namespace Crcs {
    using Crc8Dallas = ReflectedCrc<uint8_t, 0x8c, 16, 0x00, 0x00 >;
}


#endif
