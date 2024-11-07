/* Пример программы для изменения пользовательского контекста (запуска новой программы) в порожденном процессе. */

#include <stdio.h>
#include <stdlib.h>
 
#include <sys/types.h>
#include <unistd.h>

int main(const int argc, const char *argv[], const char *envp[]) {
    pid_t pid = 0;
    pid_t ppid = 0;

    int count_nesting = 0;
    int result = 0;

    pid_t chpid = fork();
    if (chpid == -1) { 
        fprintf(stderr, "Can\'t fork a child process\n");
    } else if (chpid >0) {
        /* Родительский процесс */

        count_nesting++;

        pid = getpid();
        ppid = getppid();

        printf("pid = %d, ppid = %d, count_nesting = %d\n", (int)pid, (int)ppid, count_nesting); 
    } else {
        /* Порожденный процесс */
 
        /* Мы будем запускать команду cat c аргументом командной строки без изменения параметров среды,
        т.е. фактически выполнять команду "cat имя файла", которая должна выдать содержимое данного файла на экран.
        Для функции execle в качестве имени программы мы указываем ее полное имя с путем от корневой директории.
        Первое слово в командной строке у нас должно совпадать с именем запускаемой программы. 
        Второе слово в командной строке - это имя файла, содержимое которого мы хотим распечатать.
        */


        result = execle("/bin/cat", "/bin/cat", argv[1], NULL, envp);
        if (result < 0) {
            fprintf(stderr, "Error on program start\n");
            return EXIT_FAILURE;
        }
    }

    return 0;
}

