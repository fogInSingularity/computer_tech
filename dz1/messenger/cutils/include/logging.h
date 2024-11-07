#ifndef LOGGING_H_
#define LOGGING_H_

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdbool.h>

#if !defined (STRINGIFY)
#define STRINGIFY(name_) #name_
#endif // STRINGIFY

#if !defined (TO_STR)
#define TO_STR(name_) #name_
#endif // TO_STR

typedef enum LoggingStatus {
    kLoggingStatus_Ok            = -1,
    
    kLoggingStatus_UninitEnum    =  0,

    kLoggingStatus_NullPassed    =  1,
    kLoggingStatus_CantOpenFile  =  2,
    kLoggingStatus_UninitLog     =  3,
    kLoggingStatus_InternalError =  4,
} LoggingStatus;

LoggingStatus LoggingSetup(const char* log_file_name);

#define Log(...) LogHidden(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define LogFunctionEntry() Log("%s function entry\n", __func__)
#define LogError(error_) Log("Error encountered: %s\n", STRINGIFY(error_))
#define LogVariable(format_, value_) Log(#value_ ": " format_ "\n", value_)

LoggingStatus LogHidden(const char* source_file_name,
                        const int source_line_num, 
                        const char* source_func_name, 
                        const char* format_str, 
                        ...) __attribute__(( format(printf, 4, 5)) );

const char* LogErrorToStr(LoggingStatus log_status);

#if defined (__cplusplus)
}
#endif // __cplusplus

#endif // LOGGING_H_
