#include "convert_endian.h"

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdint.h>

#define SwapByte(byte_a, byte_b)                 \
    do {                                         \
        uint8_t tmp = (byte_a);                  \
                      (byte_a) = (byte_b);       \
                                 (byte_b) = tmp; \
    } while (0)

typedef union ByteMask8 {
    uint64_t num;
    uint8_t  bytes[sizeof(uint64_t)];
} ByteMask8;

typedef union ByteMask4 {
    uint32_t num;
    uint8_t  bytes[sizeof(uint32_t)];
} ByteMask4;

typedef union ByteMask2 {
    uint16_t num;
    uint8_t  bytes[sizeof(uint16_t)];
} ByteMask2;

uint64_t ConvertOrderU64(uint64_t num) {
    ByteMask8 byte_mask;
    byte_mask.num = num;

    SwapByte(byte_mask.bytes[0], byte_mask.bytes[7]);
    SwapByte(byte_mask.bytes[1], byte_mask.bytes[6]);
    SwapByte(byte_mask.bytes[2], byte_mask.bytes[5]);
    SwapByte(byte_mask.bytes[3], byte_mask.bytes[4]);

    return byte_mask.num;
}

uint32_t ConvertOrderU32(uint32_t num) {
    ByteMask4 byte_mask;
    byte_mask.num = num;

    SwapByte(byte_mask.bytes[0], byte_mask.bytes[3]);
    SwapByte(byte_mask.bytes[1], byte_mask.bytes[2]);

    return byte_mask.num;
}

uint16_t ConvertOrderU16(uint16_t num) {
    ByteMask2 byte_mask;
    byte_mask.num = num;
    
    SwapByte(byte_mask.bytes[0], byte_mask.bytes[1]);

    return byte_mask.num;
}

uint8_t ConvertOrderU8(uint8_t num) {
    return num;
}

#if defined (__cplusplus)
}
#endif // __cplusplus
