/* Программа, иллюстрирующая использование системных вызовов open(), read() и close() для чтения информации из файла */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 60

int main(const int argc, const char* argv[]) {

    if (argc <= 1) {
        fprintf(stderr, "Not enough args\n");
    }
    
    /* Попытаемся открыть файл с именем в первом параметре вызова только
    для операций чтения */

    int file_disrc = open(argv[1], O_RDONLY);
    if (file_disrc < 0)
    {
        fprintf(stderr, "Cant open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    while (1) {
        char buffer[BUF_SIZE] = {};
        ssize_t size = read(file_disrc, buffer, BUF_SIZE);

        write(fileno(stdout), buffer, size); // printf("%s\n", buffer);
        if (size < BUF_SIZE) { break; }
    }

    int close_status = close(file_disrc); 
    if(close_status < 0) {
        fprintf(stderr, "Cant close file\n");
        return EXIT_FAILURE;
    }
    
    return 0;
} 
