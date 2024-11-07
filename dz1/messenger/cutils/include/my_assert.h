#ifndef MY_ASSERT_H_
#define MY_ASSERT_H_

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdbool.h>

#include "attr.h"

#define StaticAssert(condition_) UNUSED typedef char StaticAssertHiddenTypedef[(!!(condition_)) ? 1 : -1] 

#if defined (DEBUG)
#define AssertStrict(condition_, diagnostic_mes_) AssertStrictHidden(condition_, diagnostic_mes_, \
                                                                     #condition_, __FILE__, __LINE__, __func__)

#define AssertSoft(condition_, diagnostic_mes_) AssertionSoftHidden(condition_, diagnostic_mes_, \
                                                                    #condition_, __FILE__, __LINE__, __func__)

#else
#define AssertStrict(condition_, diagnostic_mes_) do { } while(0)
#define AssertSoft(condition_, diagnostic_mes_) (true)
#endif // DEBUG

void AssertStrictHidden(bool condition, const char* diagnostic_mes, 
                        const char* condition_str, const char* source_file, int source_line, const char* source_func);

bool AssertSoftHidden(bool condition, const char* diagnostic_mes, 
                      const char* condition_str, const char* source_file, int source_line, const char* source_func);

#if defined (__cplusplus)
}
#endif // __cplusplus

#endif // MY_ASSERT_H_
