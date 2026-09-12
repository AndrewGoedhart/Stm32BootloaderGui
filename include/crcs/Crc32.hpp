#ifndef COMMON_CRCS_INCLUDE_CRCS_CRC32_HPP
#define COMMON_CRCS_INCLUDE_CRCS_CRC32_HPP
#include <crcs/Crc.hpp>
namespace Crcs {
/// @brief CRC32 is based on IEEE-802.3 CRC32 Ethernet Standard
/// @details Initial Value 0xFFFFFFFF, input reflected i.e. read bit by bit msb first, Result reflected.
///   Polynomial  is 0x04c11db7, Final value is XOR'ed with 0xFFFFFFFF
///   This corresponds to the CRC hardware unit on the STM32F103. * Also corresponds to CRC32/ISO-HDLC algorithm
///
  using Crc32 = ReflectedCrc<uint32_t, 0xEDB88320, 16, 0xFFFFFFFF, 0xFFFFFFFF >;
}

#endif