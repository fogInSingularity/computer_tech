#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

/* Программа посылающая себе сигнал на уничтожение.*/
/* Дописать игнорирование данного сигнала.*/
int main(void)
{
    pid_t dpid = getpid();

    signal(SIGABRT, SIG_IGN);

    if (kill(dpid, SIGABRT) == -1) {
        fprintf(stderr, "Cannot send signal\n");
        return 1;
    }

    getchar();

    printf("Looks like proccess havent been killed\n");

    return 0;
}
