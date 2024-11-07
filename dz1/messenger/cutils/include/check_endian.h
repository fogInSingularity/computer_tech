#ifndef CHECK_ENDIAN_H_
#define CHECK_ENDIAN_H_

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

typedef enum EndianOrder {
    kUnknownOrder = 0,
    kBigEndian    = 1,
    kLittleEndian = 2,
} EndianOrder;

__attribute__((unused))
static const char* kUnknownOrderStr = "unknown order";
__attribute__((unused))
static const char* kBigEndianStr    = "big endian";
__attribute__((unused))
static const char* kLittleEndianStr = "little endian";

EndianOrder CheckEndianOrder(void);
const char* CheckEndianOrderStr(void);

#if defined (__cplusplus)
}
#endif // __cplusplus

#endif // CHECK_ENDIAN_H_
