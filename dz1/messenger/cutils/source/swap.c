#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include "swap.h"

#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <assert.h>

size_t SwapBytes(void* data_a, void* data_b, size_t size) {
    assert(data_a != NULL);
    assert(data_b != NULL);

    if ((data_a == NULL) || (data_b == NULL)) { return 0; }
    
    if (data_a == data_b) { return size; }

    size_t n_full_ops = (size >> 3);    // колво шагов по 8
    size_t trailer    = size & 0b111UL; // оставшиеся 7 байт

    uint8_t* a_move = (uint8_t*)data_a;
    uint8_t* b_move = (uint8_t*)data_b;
    for (size_t i = 0; i < n_full_ops; i++) {
        uint64_t temp = 0;
        memcpy(&temp, a_move, sizeof(uint64_t));
               memcpy(a_move, b_move, sizeof(uint64_t));
                       memcpy(b_move, &temp, sizeof(uint64_t));
        
        a_move += sizeof(uint64_t);
        b_move += sizeof(uint64_t);
    }

    if (trailer & 0b100UL) {
        uint32_t temp = 0;
        memcpy(&temp, a_move, sizeof(uint32_t));
               memcpy(a_move, b_move, sizeof(uint32_t));
                       memcpy(b_move, &temp, sizeof(uint32_t));
        
        a_move += sizeof(uint32_t);
        b_move += sizeof(uint32_t);
    
    }
    if (trailer & 0b010UL) {
        uint16_t temp = 0;
        memcpy(&temp, a_move, sizeof(uint16_t));
               memcpy(a_move, b_move, sizeof(uint16_t));
                       memcpy(b_move, &temp, sizeof(uint16_t));
        
        a_move += sizeof(uint16_t);
        b_move += sizeof(uint16_t);
    }

    if (trailer & 0b001UL) {
        uint8_t temp = 0;
        memcpy(&temp, a_move, sizeof(uint8_t));
               memcpy(a_move, b_move, sizeof(uint8_t));
                       memcpy(b_move, &temp, sizeof(uint8_t));
    }

    return size;
}

#if defined (__cplusplus)
}
#endif // __cplusplus
