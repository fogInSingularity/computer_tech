/* добавить вторую функцию, сравнивающую количество букв,
   добавить вызов различных функций в зависимости от аргумента программы
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int StrlenCmp(const char* str1, const char* str2);

typedef int (*StrCmpFunction)(const char * str1, const char * str2);

void check(const char* str1, const char* str2, StrCmpFunction cmp)
{
    assert(str1 != NULL);
    assert(str2 != NULL);
    assert(cmp != NULL);

    printf("Проверка на совпадение.\n");

    if (!cmp(str1, str2)) { 
        printf("Равны"); 
    } else { 
        printf("Не равны"); 
    }
}

#define kDefaultSize 80

const char* kStrcmpFlag = "strcmp";
const char* kStrlenFlag = "strlen";

int main(const int argc, const char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Error: not enough files\n");
        return EXIT_FAILURE;
    }

    char str1[kDefaultSize] = {0};
    char str2[kDefaultSize] = {0};
    
    printf("Введите две строки.\n");
    fgets(str1, kDefaultSize, stdin);
    fgets(str2, kDefaultSize, stdin);

    if (!strcmp(argv[1], kStrcmpFlag)) {
        check(str1, str2, strcmp);
    } else if (!strcmp(argv[1], kStrlenFlag)) {
        check(str1, str2, StrlenCmp);
    } else {
        fprintf(stderr, "Error: unknown flag: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return 0;
}

int StrlenCmp(const char* str1, const char* str2) {
    assert(str1 != NULL);
    assert(str2 != NULL);    

    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);

    return (int64_t)str1_len - (int64_t)str2_len;
}
