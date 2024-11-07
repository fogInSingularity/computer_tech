#ifndef CONVERT_ENDIAN_HPP_
#define CONVERT_ENDIAN_HPP_

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdint.h>

uint64_t ConvertOrderU64(uint64_t num);
uint32_t ConvertOrderU32(uint32_t num);
uint16_t ConvertOrderU16(uint16_t num);
uint8_t  ConvertOrderU8 (uint8_t  num);

#if defined (__cplusplus)
}
#endif // __cplusplus

#endif // CONVERT_ENDIAN_HPP_
