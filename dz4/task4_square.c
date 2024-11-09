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

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "debug.h"

#define SQUARE_INTRO "square: "

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, SQUARE_INTRO "not enough arguments: %d\n", argc);

        return EXIT_FAILURE;
    }
    
    int pipe_read_id = (int)strtol(argv[1], NULL, 10);
    DEBUG_PRINT(SQUARE_INTRO "pipe pid: str[%s], int[%d]\n", argv[1], pipe_read_id);

    int user_num = 0;

    int readed = 0;
    do {
        readed = read(pipe_read_id, &user_num, sizeof(user_num));
        if (readed < 0) { 
            perror(SQUARE_INTRO "cant read from pipe"); 
        }
        DEBUG_PRINT(SQUARE_INTRO "readed: %d\n", readed);
    } while (readed == 0);

    printf(SQUARE_INTRO "square of number: %d\n", user_num * user_num);

    DEBUG_PRINT(SQUARE_INTRO "exit\n");
}
