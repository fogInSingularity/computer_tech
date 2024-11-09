/* Программа для чтения из  FIFO */
/* Для отладки использовать утилиту strace: strace -e trace=open,read ./имя программы */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 20

int main(void) {
    const char fifo_name[]="meow.fifo";
    int fifo_file = open(fifo_name, O_RDONLY);
    if(fifo_file < 0) {
        perror("Can\'t open FIFO for reading");

        return EXIT_FAILURE;
    }

    /* Пробуем прочитать из FIFO 20 байт в массив,
    т.е. заполнить весь доступный буфер */
    char restring[BUFFER_SIZE] = {0};
    ssize_t readed = read(fifo_file, restring, BUFFER_SIZE);
    if(readed < 0) {
        perror("Can\'t read string");
        close(fifo_file);

        return EXIT_FAILURE;
    }

    printf("Restring: %s\n", restring);

    close(fifo_file);
    
    return 0;
} 
