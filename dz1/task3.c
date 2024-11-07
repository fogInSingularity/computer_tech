/* Пример создания нового процесса с разной работой процессов ребенка и родителя */

#include <stdio.h>
#include <stdlib.h>
 
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    pid_t pid = 0; 
    pid_t ppid = 0;
    pid_t chpid = 0;

    int count_nesting = 0;

    chpid = fork();
    if (chpid < 0) {
        fprintf(stderr, "Error\n");
        return EXIT_FAILURE;
    }
    if (chpid == 0) { 
        getchar();

        pid = getpid();
        ppid = getppid();
        
        /* Порожденный процесс */
        count_nesting++;
        printf("Child proccess:\n"
               "pid = %d, ppid = %d, count_nesting = %d\n", (int)pid, (int)ppid, count_nesting);
    } else {
        pid = getpid();
        ppid = getppid();

        printf("Parent proccess:\n"
               "pid = %d, ppid = %d, count_nesting = %d\n", (int)pid, (int)ppid, count_nesting); 
    }

    return 0;
}
