#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

/*Восстановить нормальное поведение программы через 5 принятых сигналов.*/
sig_atomic_t sig_occured = 0;
void SigHandler(int snum) {
    sig_occured = 1;
}

int main(void) {
    struct sigaction act = {};
    sigemptyset(&act.sa_mask);
    act.sa_handler = &SigHandler;
    act.sa_flags = 0;

    int sig_res = sigaction(SIGINT, &act, NULL);
    if (sig_res == -1) {
        fprintf(stderr, "sigaction() error\n");
        return EXIT_FAILURE;
    }

    int sig_counter = 0;

    while (1) {
        if (sig_occured) {
            fprintf(stderr, "signal...\n");
            sig_occured = 0;
            sig_counter++;
        }

        if (sig_counter >= 5) {
            break;
        }
    }

    printf("Exiting..\n");

    return 0;
}
