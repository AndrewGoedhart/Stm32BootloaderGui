#ifndef COMMON_TYPECONVERSION_INCLUDE_TYPECONVERSION_CONVERTTYPES_HPP
#define COMMON_TYPECONVERSION_INCLUDE_TYPECONVERSION_CONVERTTYPES_HPP

#include <cstdint>

/// @brief convert a sequence of bytes in to a 32Bit unsigned int reading LSB first
template< typename T >
uint32_t toUint32(T &bytes);

/// @brief convert a sequence of bytes in to a 16Bit unsigned int reading LSB first
template< typename T >
uint16_t toUint16(T &bytes);

/// @brief convert a sequence of bytes in to a 32Bit signed int reading LSB first
template< typename T >
int32_t toInt32(T &bytes);

/// @brief convert a sequence of bytes in to a 16Bit signed int reading LSB first
template< typename T >
int16_t toInt16(T &bytes);

/// @brief convert a sequence of bytes in to a 32Bit unsigned int reading LSB first
template< typename T >
uint32_t toUint32(const T &bytes);

/// @brief convert a sequence of bytes in to a 16Bit unsigned int reading LSB first
template< typename T >
uint16_t toUint16(const T &bytes);

/// @brief convert a sequence of bytes in to a 32Bit signed int reading LSB first
template< typename T >
int32_t toInt32(const T &bytes);

/// @brief convert a sequence of bytes in to a 16Bit signed int reading LSB first
template< typename T >
int16_t toInt16(const T &bytes);

/// @brief convert a 32 bit unsigned int to a sequence of bytes LSB first
template< typename T >
void toUint8(T &bytes, uint32_t value);

/// @brief convert a 16 bit unsigned int to a sequence of bytes LSB first
template< typename T >
void toUint8(T &bytes, uint16_t value);

/// @brief convert a 32 bit signed int to a sequence of bytes LSB first
template< typename T >
void toUint8(T &bytes, int32_t value);

/// @brief convert a 16 bit signed int to a sequence of bytes LSB first
template< typename T >
void toUint8(T &bytes, int16_t value);

/// Implementations
/// ---------------
template< typename T >
uint32_t toUint32(T &bytes) {
  uint32_t result = *bytes;
  ++bytes;
  result |= static_cast<const uint32_t>(*bytes) << 8U;
  ++bytes;
  result |= static_cast<const uint32_t>(*bytes) << 16U;
  ++bytes;
  result |= static_cast<const uint32_t>(*bytes) << 24U;
  ++bytes;
  return result;
}

template< typename T >
uint32_t toUint32(const T &bytes) {
  auto iter = bytes;
  uint32_t result = *iter;
  ++iter;
  result |= static_cast<const uint32_t>(*iter) << 8U;
  ++iter;
  result |= static_cast<const uint32_t>(*iter) << 16U;
  ++iter;
  result |= static_cast<const uint32_t>(*iter) << 24U;
  return result;
}

template< typename T >
uint16_t toUint16(T &bytes) {
  uint16_t result = *bytes;
  ++bytes;
  result = static_cast<uint16_t>(result | (static_cast<const uint32_t>(*bytes) << 8U));
  ++bytes;
  return result;
}

template< typename T >
uint16_t toUint16(const T &bytes) {
  auto iter = bytes;
  uint16_t result = *iter;
  ++iter;
  result |= static_cast<const uint32_t>(*iter) << 8U;
  return result;
}

template< typename T >
void toUint8(T &bytes, const uint32_t value) {
  *bytes = static_cast<uint8_t>(value);
  ++bytes;
  *bytes = static_cast<uint8_t>(value >> 8);
  ++bytes;
  *bytes = static_cast<uint8_t>(value >> 16);
  ++bytes;
  *bytes = static_cast<uint8_t>(value >> 24);
  ++bytes;
}

template< typename T >
constexpr void toUint8(const T &bytes, const uint32_t value) {
  auto iter = bytes;
  *iter = static_cast<uint8_t>(value);
  ++iter;
  *iter = static_cast<uint8_t>(value >> 8);
  ++iter;
  *iter = static_cast<uint8_t>(value >> 16);
  ++iter;
  *iter = static_cast<uint8_t>(value >> 24);
}


template< typename T >
void toUint8(T &bytes, const uint16_t value) {
  *bytes = static_cast<uint8_t>(value);
  ++bytes;
  *bytes = static_cast<uint8_t>(value >> 8);
  ++bytes;
}

template< typename T >
void toUint8(const T &bytes, const uint16_t value) {
  auto iter = bytes;
  *iter = static_cast<uint8_t>(value);
  ++iter;
  *iter = static_cast<uint8_t>(value >> 8);
}

template< typename T >
int32_t toInt32(T &bytes) {
  return static_cast<int32_t>(toUint32(bytes));
}

template< typename T >
int32_t toInt32(const T &bytes) {
  return static_cast<int32_t>(toUint32(bytes));
}

template< typename T >
int16_t toInt16(T &bytes) {
  return static_cast<int16_t>(toUint16(bytes));
}

template< typename T >
int16_t toInt16(const T &bytes) {
  return static_cast<int16_t>(toUint16(bytes));
}

template< typename T >
inline void toUint8(T &bytes, const int32_t value) {
  toUint8(bytes, static_cast<uint32_t>(value));
}

template< typename T >
inline void toUint8(T &bytes, const int16_t value) {
  toUint8(bytes, static_cast<uint16_t>(value));
}

inline bool toBool(const uint32_t val) {
  return val == 1 ? true : false;
}

inline uint32_t toInt(const bool val) {
  return val ? 1 : 0;
}

#endif

