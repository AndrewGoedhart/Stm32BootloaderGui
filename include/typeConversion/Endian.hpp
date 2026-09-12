#ifndef COMMON_TYPECONVERSION_INCLUDE_TYPECONVERSION_ENDIAN_HPP
#define COMMON_TYPECONVERSION_INCLUDE_TYPECONVERSION_ENDIAN_HPP

#include <cstdint>

namespace Utils {
  inline uint16_t swapEndian(uint16_t value){
    return __builtin_bswap16(value);
  }
  inline int16_t swapEndian(int16_t value){
    return static_cast<int16_t>(__builtin_bswap16(static_cast<uint16_t>(value)));
  }
  inline uint32_t swapEndian(uint32_t value){
    return __builtin_bswap32(value);
  }
  inline int32_t swapEndian(int32_t value){
    return static_cast<int32_t>(__builtin_bswap32(static_cast<uint32_t>(value)));
  }
}



#endif 
