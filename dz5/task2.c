/* Модифицировать программу, 
 * чтобы замерить среднее время завершения нити после сигнала на завершение.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <pthread.h>
#include <unistd.h>

void* AnyFunc(void* arg);

void* AnyFunc(void* arg) {
    // ignore arg

    while (1) {
        // fputc('.', stderr);
  	    sleep(10);
    }

    return NULL;
}

int main(const int argc, const char* argv[]) {  
    if (argc < 2) {
        fprintf(stderr, "Not enough args\n");

        return EXIT_FAILURE;
    }
    
    size_t n_iters = (size_t)strtol(argv[1], NULL, /*base:*/ 10);
    size_t full_time = 0;

    for (size_t i = 0; i < n_iters; i++) {
    
        pthread_t thread = {};
        void* result = NULL;

        int try_create = pthread_create(&thread, NULL, &AnyFunc, NULL);
        if (try_create != 0) {
  	        fprintf(stderr, "Error will attempting to create pthread: %s\n", strerror(try_create));

  	        return EXIT_FAILURE;
        }

        pthread_cancel(thread);
        clock_t start_cancel_time = clock();     

        pthread_join(thread, &result);
        assert(result == PTHREAD_CANCELED);

        clock_t end_cancel_time = clock();

        full_time += end_cancel_time - start_cancel_time;
    }

    printf("avg time in ticks: %lu\n", full_time / n_iters);

    return 0;
}
