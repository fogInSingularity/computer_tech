/* Программа для записи в FIFO*/
/* Для отладки использовать утилиту strace: strace -e trace=open,read ./имя программы */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    // Обнуляем маску создания файлов текущего процесса для того, чтобы
    // права доступа у создаваемогоFIFO точно соотвествовали параметру вызова mknod()
    (void)umask(0);

    // создаём fifo
    const char fifo_name[]="meow.fifo";
    const mode_t full_access = 0666;
    int create_res = mknod(fifo_name, S_IFIFO | full_access, 0);
    if(create_res < 0) {
        perror("Can\'t create FIFO\n");

        return EXIT_FAILURE;
    }

    /* Открываем FIFO на запись.*/
    int fifo_file = open(fifo_name, O_WRONLY);
    if (fifo_file < 0) {
        perror("Can\'t open FIFO for writting\n");
        close(fifo_file);

        return EXIT_FAILURE;
    }

    /* Пробуем записать в FIFO ?? байт, т.е. всю строку "Погладь Кота!"
        вместе с ... */
    const char pet_the_cat[] = "Pet the cat\n";
    size_t size = write(fifo_file, pet_the_cat, strlen(pet_the_cat));
    if(size != strlen(pet_the_cat)) {
        fprintf(stderr, "Can\'t write all string to FIFO\n");
        close(fifo_file);

        return EXIT_FAILURE;
    }

    close(fifo_file);
    return 0;
} 