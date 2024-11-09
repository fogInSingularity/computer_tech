/* Создать программный комплекс для возведения числа в квадрат
(или выполнения любой другой функции).*/
/* Перая программа создаёт или удаляет трубу.*/
/* Вторая программа(сервер) получает из трубы значение, возводит в квадрат
и печатает его.*/
/* Третья программа (клиент) читает с терминала число и пихает его в трубу.*/
/* Если ввести не число, то клиент и сервер завершают работу.*/

/* Дополнительно усовершенствовать задачу,
   создать вторую трубу для обратной связи.
   (третья программа печатает число - результат)*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <unistd.h>
#include <sys/wait.h>

#define READ_IND 1 // NOTE why
#define WRITE_IND 0

const char square_proc_file_name[] = "run4_read_term";
const char read_term_proc_file_name[] = "run4_square";

#define BUFFER_SIZE 64

int main(void) {
    int pipe_discr[2] = {};
    if (pipe(pipe_discr) < 0) {
        perror("Pipe manager cant open pipe:");

        return EXIT_FAILURE;
    }

    int proc_id_read_term = fork();
    if (proc_id_read_term == 0) { // child
        char pid_to_str[BUFFER_SIZE] = {0};
        snprintf(pid_to_str, BUFFER_SIZE, "%d", pipe_discr[WRITE_IND]);

        execl(read_term_proc_file_name, read_term_proc_file_name, pid_to_str, NULL);
        assert(0 && "should get to this point");
    }

    int proc_id_square = fork();
    if (proc_id_square == 0) { // child
        char pid_to_str[BUFFER_SIZE] = {0};
        snprintf(pid_to_str, BUFFER_SIZE, "%d", pipe_discr[READ_IND]); 

        execl(square_proc_file_name, square_proc_file_name, pid_to_str, NULL);
        assert(0 && "should get to this point");
    }

    waitpid(proc_id_read_term, NULL, 0);
    waitpid(proc_id_square, NULL, 0);

#if defined (DEBUG)
    fprintf(stderr, "pipe manager exit\n");
#endif // DEBUG
}
