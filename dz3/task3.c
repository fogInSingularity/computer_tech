#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>

// FIXME не доделал

volatile sig_atomic_t sig_status = 0;

// 1 Обработкчик прерывания 
void HandleUsr1(int s_num) {
    fputs("handle_usr1\n", stderr);
    sig_status = 1;
}

// 2 Обработкчик прерывания 
void HandleUsr2(int s_num) {
    fputs("handle_usr2\n", stderr);
    sig_status = 2;
}

int main(const int argc, const char ** argv) {
    if (argc < 2) {
        fprintf (stderr, "Too few arguments\n");
        return EXIT_FAILURE;
    }	

    // sigaction описывает что сделать когда приходит сигнал

    struct sigaction act_usr1;
    sigemptyset(&act_usr1.sa_mask);
    act_usr1.sa_flags = 0;
    act_usr1.sa_handler = &HandleUsr1;
    int sigact_res1 = sigaction(SIGUSR1, &act_usr1, NULL);
    if (sigact_res1 == -1) {
        fprintf(stderr, "sigaction (act_usr1) error\n");
        return EXIT_FAILURE;
    }

    struct sigaction act_usr2;
    sigemptyset(&act_usr2.sa_mask);
    act_usr2.sa_flags = 0;
    act_usr2.sa_handler = &HandleUsr2;
    int sigact_res2 = sigaction(SIGUSR2, &act_usr2, NULL);
    if (sigact_res2 == -1) {
        fprintf(stderr, "sigaction (act_usr2) error\n");
        return EXIT_FAILURE;
    }

    pid_t new_pid = fork();
    if (!new_pid) { // child:
        execl("run3_helper", "run3_helper", argv[1], NULL);
       
        // execl only returns if error ocurs    
        perror("Execl fail:");
        kill(getppid(), SIGABRT);
        return EXIT_FAILURE;
    }

    while (1) {
        if (sig_status == 1) {
            printf("%s: leap year\n", argv[1]);
            return 0;
        }

        if (sig_status == 2) {
            printf("%s: not leap year\n", argv[1]);
            return 0;
        }
    }
    
    return 0;
}
