#include <stdio.h>
 
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
 
    printf("My pid = %d, my ppid = %d\n", pid, ppid);
}
