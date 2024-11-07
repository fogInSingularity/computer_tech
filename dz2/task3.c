#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>

const size_t kShmemSize = 4096;
const char* kShmemMessage = "Poglad kota\n";

int main(void) {
    int shared_memory_id = shmget(IPC_PRIVATE, // не поле флага, а тип key_t. Если key равно этому специальному значению, 
                                              // то системный вызов игнорирует всё кроме 9-ти младших  битов  shmflg  и  создаёт  новый  общий сегмент памяти.
                                  kShmemSize, // shared memory size  
                                  IPC_CREAT // создаёт новый сегмент
                                  | IPC_EXCL // создаёт новый сегмент(?)
                                  | 0600 // read and write for owner
                                 );
    if (shared_memory_id < 0) {
        perror("Cant get shared memory");

        return EXIT_FAILURE;
    }

    uint8_t* shared_memory = shmat(shared_memory_id, // attaches memory
                                   NULL, // automaticly choose page 
                                   0 // no flags
                                  );
    if (shared_memory == (uint8_t*)(-1)) {
        perror("Cant attach memory");

        return EXIT_FAILURE;
    }

    struct shmid_ds shared_memory_header = {};
    int shmctl_res = shmctl(shared_memory_id, IPC_STAT, &shared_memory_header);
    if (shmctl_res < 0) {
        perror("Cant get stat of shared memory");
        shmdt(shared_memory);
    
        return EXIT_FAILURE;
    }

    size_t segment_size = shared_memory_header.shm_segsz; 
    if (segment_size < strlen(kShmemMessage)) {
        fprintf(stderr, "Not enough memory for message\n");
        shmdt(shared_memory);

        return EXIT_FAILURE;
    }

    strcpy((char*)shared_memory, kShmemMessage);

    printf("shared_memory_id: %d\n", shared_memory_id);
    puts("Wainting..\n");
    getchar();

    shmdt(shared_memory);
    shmctl(shared_memory_id, 
           IPC_RMID, // mark segment to be destroyed
           NULL
          );
}
