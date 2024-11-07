#include "my_assert.h"

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

#include <stdio.h>

#include "logging.h"

void AssertStrictHidden(bool condition, const char* diagnostic_mes, 
                        const char* condition_str, const char* source_file, int source_line, const char* source_func) 
{ 
    if (condition) { return ; }

    Log("Strict assertion failed:\n"
        "\tdiagnostic: %s\n"
        "\tfailed:     %s\n"
        "\tlocated:    %s:%d\n"
        "\tfunction:   %s\n", 
        diagnostic_mes,
        condition_str,
        source_file,
        source_line,
        source_func);
 
    fprintf(stderr,
            "Strict assertion failed:\n"
            "\tdiagnostic: %s\n"
            "\tfailed:     %s\n"
            "\tlocated:    %s:%d\n"
            "\tfunction:   %s\n", 
            diagnostic_mes,
            condition_str,
            source_file,
            source_line,
            source_func);
}

bool AssertSoftHidden(bool condition, const char* diagnostic_mes, 
                      const char* condition_str, const char* source_file, int source_line, const char* source_func) 
{
    if (condition) { return true; }
 
    Log("Soft assertion failed:\n"
        "\tdiagnostic: %s\n"
        "\tfailed:     %s\n"
        "\tlocated:    %s:%d\n"
        "\tfunction:   %s\n", 
        diagnostic_mes,
        condition_str,
        source_file,
        source_line,
        source_func);
 
    fprintf(stderr,
            "Soft assertion failed:\n"
            "\tdiagnostic: %s\n"
            "\tfailed:     %s\n"
            "\tlocated:    %s:%d\n"
            "\tfunction:   %s\n", 
            diagnostic_mes,
            condition_str,
            source_file,
            source_line,
            source_func);   

    return false;
}

#if defined (__cplusplus)
}
#endif // __cplusplus

