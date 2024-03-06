#ifndef COMMON_TYPECONVERSION_INCLUDE_TYPECONVERSION_BITS_HPP
#define COMMON_TYPECONVERSION_INCLUDE_TYPECONVERSION_BITS_HPP
#include <cstdint>

namespace Utils {
/// @brief Returns true if the number is a power of 2
  constexpr uint32_t isPower2(const uint32_t value) {
    return (value & (value - 1)) == 0;
  }

/// @brief Returns the number of bits required to hold N values
/// @param size the number of values
/// @return the smallest number of bits that can hold N values
constexpr uint32_t computeBitSize(uint32_t size) {
    if (size == 0) {
      return 0;
    }
    size = size - 1;
    uint32_t bits = 0;
    do {
      size = size >> 1;
      bits = bits + 1;
    } while (size > 0);
    return bits;
  }

  /// @brief Returns a mask of N bits set to 1
  /// @param bits the number of bits set to 1 in the mask
  /// @return the mask
  constexpr uint32_t generateMask(const uint32_t bits) {
    constexpr uint32_t mask = 0xFFFFFFFFU;
    return (mask >> (32 - bits));
  }


/// @brief The number of n bit slices that fit into a byte.
/// @param bits the number of bits
/// @return the number of n bit slices that fit into the byte. So 4 2 bit slices fit but 2 3 bit slices.
  constexpr uint32_t computeSlicesPerByte(const uint32_t bits) {
    return 8 / bits;
  }
}

#endif
