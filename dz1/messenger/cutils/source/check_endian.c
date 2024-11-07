#include "check_endian.h"

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

EndianOrder CheckEndianOrder(void) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return kLittleEndian;
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return kBigEndian;
    #else
        return kUnknownOrder;
    #endif // __BYTE_ORDER__
}

const char* CheckEndianOrderStr(void) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return kLittleEndianStr;
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return kBigEndianStr;
    #else
        return kUnknownOrderStr;
    #endif // __BYTE_ORDER__
}

#if defined (__cplusplus)
}
#endif // __cplusplus
