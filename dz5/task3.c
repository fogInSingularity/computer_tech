/* Переделать программу для доказательстава: sin*sin + cos*cos ==1
 * Квадрат синуса считать в одном потоке, косинуса во втором,
 * а результат суммировать в главной программе.*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <pthread.h>
#include <unistd.h>

void* ThreadFunc1(void* arg);
void* ThreadFunc2(void* arg);

// run3 <angle in deg>
int main (const int argc, const char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Not enough args\n");

        return EXIT_FAILURE;
    }

    assert(sizeof(double) == sizeof(void*));

    long angle_in_deg = strtol(argv[1], NULL, /*base:*/ 10);
    double angle_in_rad = ((double)angle_in_deg / 180) * M_PI;  

    void* angle_in_rad_casted = NULL;
    memcpy(&angle_in_rad_casted, &angle_in_rad, sizeof(angle_in_rad_casted));

    pthread_t thread1 = {0};
    pthread_t thread2 = {0};

    int try_create = pthread_create(&thread1, NULL, &ThreadFunc1, angle_in_rad_casted);
    if (try_create != 0)
    {
        fprintf(stderr, "Error (thread1): %s\n", strerror(try_create));

        return EXIT_FAILURE;
    }

    try_create = pthread_create(&thread2, NULL, &ThreadFunc2, angle_in_rad_casted);
    if (try_create != 0) 
    {
        fprintf (stderr, "Error (thread2)\n");
        return EXIT_FAILURE;
    }

    double cos_sq = 0;
    double sin_sq = 0;

    pthread_join(thread1, (void**)&cos_sq);
    pthread_join(thread2, (void**)&sin_sq);

    double sum = cos_sq + sin_sq;
    printf("result cos^2 + sin^2 = %f\n", sum);

    return 0;
}

void* ThreadFunc1(void* arg) {
    double rad = 0;
    
    memcpy(&rad, &arg, sizeof(rad));

    double cos_sq = cos(rad) * cos(rad);

    void* res = NULL;
    memcpy(&res, &cos_sq, sizeof(res));
        
    return res;
}

void* ThreadFunc2(void* arg) {
    double rad = 0;
    
    memcpy(&rad, &arg, sizeof(rad));

    double sin_sq = sin(rad) * sin(rad);

    void* res = NULL;
    memcpy(&res, &sin_sq, sizeof(res));
        
    return res;
}


