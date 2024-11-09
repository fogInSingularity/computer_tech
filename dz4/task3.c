// NOTE почему то нужно закрывать все в конце, нельзя закрыть часть сначала, часть в конце

/* Программа, осуществляющая двунаправленную связь через pipe
между процессом-родителем и процессом-ребенком */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include "debug.h"

#define BUFFER_SIZE 80
#define READ_IND 0
#define WRITE_IND 1

int main()
{
    int pipe_discr1[2] = {0};
    int pipe_discr2[2] = {0};

    ssize_t size;
    char restring[BUFFER_SIZE] = {0};

    if((pipe(pipe_discr1) < 0) || (pipe(pipe_discr2) < 0)) {
        perror("Can\'t create pipe");
        
        return EXIT_FAILURE;
    }

    int result = fork();
    if(result < 0) {
        perror("Can\'t fork child");

        return EXIT_FAILURE;
    }

    const char* exiting_message = "<uninit>\n";
    if (result > 0) {
        // parent proccess
        // close(pipe_discr1[READ_IND]);
        // close(pipe_discr2[WRITE_IND]);
        // 
        /* Пишем в первый pipe и читаем из второго */ 

        const char mes_child[] = "Привет, дитя!\n";
        size = write(pipe_discr1[WRITE_IND], mes_child, strlen(mes_child) + 1);
        if(size != strlen(mes_child) + 1) {
            fprintf(stderr, "Parent: can\'t write all string\n");
        
            return EXIT_FAILURE;
        }

        size = read(pipe_discr2[READ_IND], restring, BUFFER_SIZE);
        if (size < 0) {
            perror("Parent: can\'t read string");
        
            return EXIT_FAILURE;
        }
        
        fprintf(stderr, "Parent receive: %s\n", restring); 

        close(pipe_discr1[WRITE_IND]);
        close(pipe_discr2[READ_IND]);
        
        // ?
        close(pipe_discr1[READ_IND]);
        close(pipe_discr2[WRITE_IND]);

        exiting_message = "Parent exit\n";
    } else {
        // close(pipe_discr1[WRITE_IND]);
        // close(pipe_discr2[READ_IND]);

        /* Читаем из первого pip'а и пишем во второй */
        size = read(pipe_discr1[READ_IND], restring, BUFFER_SIZE);
        if (size < 0) {
            perror("Child: can\'t read string");
        
            return EXIT_FAILURE;
        }
  
        fprintf(stderr, "Child receive: %s\n", restring);

        const char mes_parent[] = "Ну привет, папик!\n";
        size = write(pipe_discr2[WRITE_IND], mes_parent, strlen(mes_parent) + 1);
        if(size != strlen(mes_parent) + 1) {
            fprintf(stderr, "Child: can\'t write all string\n");
        
            return EXIT_FAILURE;
        }

        close(pipe_discr1[READ_IND]);
        close(pipe_discr2[WRITE_IND]);

        // ? 
        close(pipe_discr1[WRITE_IND]);
        close(pipe_discr2[READ_IND]);

        exiting_message = "Child exit\n";
    }

#if defined (DEBUG)
        fputs(exiting_message, stderr);
#endif // DEBUG

    return 0;
} 
