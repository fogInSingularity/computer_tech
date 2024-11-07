#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/shm.h>

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Not enough args\n");
    
        return EXIT_FAILURE;
    }

    int shared_memory_id = atoi(argv[1]);
    uint8_t* shared_memory = shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (uint8_t*)(-1)) {
        perror("cant attach memory");

        return EXIT_FAILURE;
    }

    printf("%s\n", shared_memory);

    shmdt(shared_memory);

    return 0;
}
