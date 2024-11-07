#include "file_wraper.h"

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "logging.h"

#undef fopen
#undef fclose

static int64_t file_opened = 0;

FILE* FOpenWraperHidden(const char* file_name, const char* open_mode, 
                        const char* source_file, int source_line, const char* source_func) {
    assert(file_name   != NULL);
    assert(open_mode   != NULL);
    assert(source_file != NULL);
    assert(source_func != NULL);
    assert(source_line >= 0);

    LogFunctionEntry();
    Log("fopen args: %s: %s, %s: %s\n", STRINGIFY(file_name), file_name, STRINGIFY(open_mode), open_mode);
    Log("call located: %s:%d\n", source_file, source_line);
    Log("call from function: %s\n", source_func);
    Log("begin of the call: %s: %ld\n", STRINGIFY(file_opened), file_opened);
    
    FILE* try_open_file = fopen(file_name, open_mode);
    if (try_open_file == NULL) {
        Log("unable to open file: %s\n", file_name);
    } else {
        Log("file had been open succesfuly: %s: %p\n", file_name, try_open_file);
        file_opened++;
    }

    Log("end of the call: %s: %ld\n", STRINGIFY(file_opened), file_opened);

    return try_open_file;
}

int FCloseWraperHidden(FILE* file, 
                       const char* source_file, int source_line, const char* source_func) {
    // can file be NULL?
    assert(source_file != NULL);
    assert(source_func != NULL);
    assert(source_line >= 0);

    LogFunctionEntry();
    Log("fclose args: %s: %p\n", STRINGIFY(file), file);
    Log("call located: %s:%d\n", source_file, source_line);
    Log("call from function: %s\n", source_func);
    Log("begin of the call: %s: %ld", STRINGIFY(file_opened), file_opened);

    int try_close_file = fclose(file); 
    if(try_close_file) {
        Log("unable to close file: %d\n", try_close_file);
    } else {
        Log("file had been close succesfuly: %p\n", file);
        file_opened--;
    }

    Log("end of the call: %s: %ld\n", STRINGIFY(file_opened), file_opened);
    
    return try_close_file;
}

#if defined (__cplusplus)
}
#endif // __cplusplus

