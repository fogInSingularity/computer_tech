#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

// Написать комментарии, отладить работу
int main(const int argc, const char* argv[]) {
    const char pathname[] = "semaphore_meow";
    key_t key = ftok(pathname, 0);
    if (key == (key_t)(-1)) {
        perror("Cant get key");

        return EXIT_FAILURE;
    }

    const int kFullAccess = 0666;
    int semid = semget(key, 1, kFullAccess | IPC_CREAT);
    if (semid < 0) {
        perror("Can\'t create semaphore set");
        
        return EXIT_FAILURE;
    }

    // semctl(semid, 0 /* индекс семафоры */, SETVAL, 1 /* инициализировать семафор значением 1 */);
 
    // Операция над семафорой
    struct sembuf mybuf = {
        .sem_num = 0, // номер семафоры, тк всего 1 то её номер 0 
        .sem_op  = 1, // декремент условия
    //  .sem_flg = 0
        .sem_flg = IPC_NOWAIT // не блокировать если ресурс занят
    };

    int op_res = semop(semid, &mybuf, 1);
    if(op_res < 0) {
        perror("Can\'t add 1 to semaphore");
        
        return EXIT_FAILURE;
    }  
    
    printf("The condition is present\n");

	semctl (semid, 1, IPC_RMID, NULL); // release semaphore

    return 0;
}
