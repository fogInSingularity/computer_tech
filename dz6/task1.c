#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

/* Написать комментарии, отладить работу */
int main(const int argc, const char* argv[]) {
    const char pathname[] = "semaphore_meow";
    
    // создание ключа который служит внешнем именем для IPC объекта(семафора в нашем случае)
    key_t key = ftok(pathname, 0);     
    if (key == (key_t)(-1)) {
        perror("Cant generate key");
    }
    
    const int kFullAccess = 0666;
    
    // семафоры используются для доступа к общим объектам нексольких процессов
    // Получение id семафоры
    // 1 = бинарная семафора, доступ имеет только 1 процесс "at a time"
    int semid = semget(key, 1, kFullAccess | IPC_CREAT /*создаёт семафору если ещё не была создана*/); 
    if(semid < 0) {
        perror("Can\'t create semaphore set");
        
        return EXIT_FAILURE;
    }

    // init semaphore
    semctl(semid, 0 /* индекс семафоры */, SETVAL, 1 /* инициализировать семафор значением 1 */);

    // Операция над семафорой
    struct sembuf mybuf = {
        .sem_num =  0, // номер семафоры, тк всего 1 то её номер 0 
        .sem_op  = -1, // декремент условия
    //  .sem_flg = 0
        .sem_flg = IPC_NOWAIT // не блокировать если ресурс занят
    };

    // выполнение массива операций 
    int sem_res = semop(semid, &mybuf, /* количество операция */ 1); // NOTE
    if(sem_res < 0) {
        perror("Can\'t wait for condition");

        return EXIT_FAILURE;
    }  

    printf("The condition is present\n");

    semctl (semid, 1, IPC_RMID, NULL); // release semaphore
    
    return 0;
}
