#include "logging.h"

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "debug.h"

#undef fopen
#undef fclose

// static ---------------------------------------------------------------------

static FILE* log_file = NULL;

static void LoggingDtor(void);

// global ---------------------------------------------------------------------

LoggingStatus LoggingSetup(const char *log_file_name) {
    assert(log_file_name != NULL); // for debug
    if (log_file_name == NULL) { return kLoggingStatus_NullPassed; }

    FILE* try_open_log_file = fopen(log_file_name, "w");
    if (try_open_log_file == NULL) { return kLoggingStatus_CantOpenFile; }

    int set_buf_status = setvbuf(try_open_log_file, NULL, _IONBF, 0);
    if (set_buf_status) {
        fclose(try_open_log_file);
        return kLoggingStatus_InternalError; 
    }

    int atexit_status = atexit(LoggingDtor);
    if (atexit_status) {
        fclose(try_open_log_file);
        return kLoggingStatus_InternalError;
    }

    log_file = try_open_log_file;

    return kLoggingStatus_Ok;
}

LoggingStatus LogHidden(const char* source_file_name, 
                        const int source_line_num, 
                        const char* source_func_name, 
                        const char* format_str, 
                        ...) 
{
    assert(format_str       != NULL);
    assert(source_file_name != NULL);
    assert(source_line_num  >= 0);
    assert(source_func_name != NULL);

    if ((format_str == NULL)
        || (source_file_name == NULL)
        || (source_func_name == NULL)
        || (source_line_num < 0)) 
    { return kLoggingStatus_NullPassed; }
    
    if (log_file == NULL) { return kLoggingStatus_UninitLog; }

#if defined (DEBUG)
    time_t current_time = time(NULL);
    if (current_time == (time_t)-1) {
        return kLoggingStatus_InternalError;
    }
    
    struct tm* current_tm = localtime(&current_time);

    va_list args;
    va_start(args, format_str);

    fprintf(log_file, 
            "[%d:%d:%d][%s:%s:%d]:\n", 
            current_tm->tm_hour, 
            current_tm->tm_min, 
            current_tm->tm_sec, 
            source_file_name, 
            source_func_name, 
            source_line_num);
    vfprintf(log_file, format_str, args);
    fputc('\n', log_file);

    va_end(args);
#else 
    // do nothing    
#endif // DEBUG
    return kLoggingStatus_Ok;
}

const char* LogErrorToStr(LoggingStatus log_status) {
    switch (log_status) {
        case kLoggingStatus_Ok:            return STRINGIFY(kLoggingStatus_Ok);

        case kLoggingStatus_UninitEnum:    return STRINGIFY(kLoggingStatus_UninitEnum);
        
        case kLoggingStatus_NullPassed:    return STRINGIFY(kLoggingStatus_NullPassed);
        case kLoggingStatus_CantOpenFile:  return STRINGIFY(kLoggingStatus_CantOpenFile);
        case kLoggingStatus_UninitLog:     return STRINGIFY(kLoggingStatus_UninitLog);
        case kLoggingStatus_InternalError: return STRINGIFY(kLoggingStatus_InternalError); 
        default: assert(0 && "unknown enum value"); return "unknown enum value";
    }
}

// static ---------------------------------------------------------------------

static void LoggingDtor(void) {
    if (log_file == NULL) { return ; }

    LogFunctionEntry();

    fclose(log_file);
    log_file = NULL;
}

#if defined (__cplusplus)
}
#endif // __cplusplus
