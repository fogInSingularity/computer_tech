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

#include <unistd.h>

#include "debug.h"

#define READ_TERM_INTRO "read_term: "

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, READ_TERM_INTRO "not enough arguments: %d\n", argc);

        return EXIT_FAILURE;
    }

    int pipe_write_id = (int)strtol(argv[1], NULL, 10);
    DEBUG_PRINT(READ_TERM_INTRO "pipe pid: str[%s], int[%d]\n", argv[1], pipe_write_id);

    int user_number = 0;
    scanf("%d", &user_number);

    int writen = write(pipe_write_id, &user_number, sizeof(user_number));
    if (writen < 0) {
        perror(READ_TERM_INTRO "cant write to pipe");
    }

    DEBUG_PRINT(READ_TERM_INTRO "exit\n");
} 
