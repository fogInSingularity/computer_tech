// -lpthread или -pthread для успешной компиляции добавтить одну из этих опций
// Добавить вызов any_func в основную нить выполнения, чтобы распечатать 2009

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <pthread.h>

void* AnyFunc(void* arg);

void* AnyFunc(void* arg) {
    int a = *(int*)arg;// Добавить описание этой строки
    a++;

    return (void*)(size_t)a;
}

int main(void) {
    pthread_t thread = {};

    int parg = 2007;
    int try_create = pthread_create(&thread, NULL, &AnyFunc, &parg);
    if (try_create != 0) {
        fprintf (stderr, "Error on pthread_create call\n");
	      return EXIT_FAILURE;
    }

    int pdata = 0;
    pthread_join(thread, (void*)&pdata);
   
    int res = (int)(size_t)AnyFunc(&pdata);
    
    printf("result: %d\n", res);

    return 0;
}
