#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>
#include <fcntl.h>

#include "logging.h"

const int kFullAccess = 0777;
#define BUFFER_SIZE 128

int ListenOnFile(const char* file_name);
int SendToFile(const char* file_name);

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Not enough args\n");
        return EXIT_FAILURE;
    }

    LoggingStatus log_status = LoggingSetup("log.log");
    assert(log_status == kLoggingStatus_Ok);

    pid_t new_pid = fork();
    LogVariable("%d", new_pid);
    if (new_pid < -1) {
        Log("Unable to fork\n");
        
        fprintf(stderr, "Internal error\n");

        return EXIT_FAILURE;
    }

    if (new_pid == 0) { // child branch
        Log("Child branch entered\n");
    
        return SendToFile(argv[1]);
    } else {
        Log("Parent branch entered\n");
    
        return ListenOnFile(argv[1]);
    }

    Log("what a sigma\n");
}

int ListenOnFile(const char* file_name) {
    assert(file_name != NULL); 

    LogFunctionEntry();

    int messenger_fd = open(file_name, O_RDWR | O_CREAT | O_APPEND, kFullAccess); 
    LogVariable("%d", messenger_fd);
    if (messenger_fd < 0) {
        fprintf(stderr, "Unable to open file: %s", file_name);

        Log("Unable to open file: %s", file_name);
            
        return EXIT_FAILURE;
    }

    while (true) {
        char buffer[BUFFER_SIZE + 1] = {0};
        ssize_t readed = read(messenger_fd, buffer, BUFFER_SIZE);

        if (readed > 0) {
            buffer[readed] = '\0';

            LogVariable("%ld", readed);
            LogVariable("%s", buffer);

            ssize_t writen = write(fileno(stdout), buffer, (size_t)readed);
            if (writen != readed) {
                LogVariable("%ld", writen);
            }
        }
    }

    return EXIT_SUCCESS;
}

int SendToFile(const char* file_name) {
    assert(file_name != NULL);

    LogFunctionEntry();

    int messenger_fd = open(file_name, O_RDWR | O_CREAT | O_APPEND, kFullAccess); 
    LogVariable("%d", messenger_fd);
    if (messenger_fd < 0) {
        fprintf(stderr, "Unable to open file: %s", file_name);

        Log("Unable to open file: %s", file_name);
            
        return EXIT_FAILURE;
    }
 
    while (true) {
        char buffer[BUFFER_SIZE + 1] = {0};
        ssize_t readed = read(fileno(stdin), buffer, BUFFER_SIZE);
    
        if (readed > 0) {
            buffer[readed] = '\0';

            LogVariable("%ld", readed); 
            LogVariable("%s", buffer);

            ssize_t writen = write(messenger_fd, buffer, (size_t)readed);
            if (writen != readed) {
                LogVariable("%ld", writen);
            }
        }
    }
        
    return EXIT_SUCCESS;
}
