/*
    Написать программу мирно считающую факториал, а по сигналу выводящую на терминал
        текущее время счёта и промежуточный результат.
    Сигнал выдавать из другого терминала, командой терминала.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <signal.h>

// globals -----------------------

unsigned factorial_res = 0;
clock_t factorial_starting_time = 0;

// Factorial ---------------------

unsigned Factorial(unsigned n);

// signal handlers ---------------

void SigHandler(int snum);

// main --------------------------

int main(void) {
    struct sigaction act = {};
    sigemptyset (&act.sa_mask);
    act.sa_handler = &SigHandler;
    act.sa_flags = 0;

    int sig_res = sigaction(SIGINT, &act, NULL);
    if (sig_res == -1) {
        fprintf (stderr, "sigaction() error\n");
        return EXIT_FAILURE;
    }

    const unsigned n_iters = 1000000;
    for (unsigned i = 0; i < n_iters; i++) {
        Factorial(i);
    }

    return 0;
}

// Factorial ---------------------

unsigned Factorial(unsigned n) {
    factorial_res = 1;
    factorial_starting_time = clock();

    for (unsigned i = 1; i <= n; i++) {
        factorial_res *= i;
    }

    return factorial_res;
}

// signal handlers ---------------

void SigHandler(int snum) {
    fprintf(stderr, "factorial temp result: %u\n", factorial_res);
    fprintf(stderr, "time(in clocks) for factorial: %ld\n", clock() - factorial_starting_time);
}


