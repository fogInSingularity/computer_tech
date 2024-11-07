/* Программа 2 для чтения текста исходного файла из разделяемой памяти.*/
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* Предполагаем, что размер исходного файла < SIZE */
const size_t kMaxSize = 65535;
const int kFullAccess = 0666;

int main()
{
    uint8_t* shared_memory = NULL; 
    
    
    char file_name[] = "some_file"; 

    key_t ipc_key = ftok(file_name, 0);
    if(ipc_key < 0)
    {
        perror("Cant generate ipc key");

        return EXIT_FAILURE;
    }

    int shared_memory_id = shmget(ipc_key, kMaxSize, kFullAccess | IPC_CREAT);
    if(shared_memory_id < 0)
    {
        perror("Cant create shared memory");
        
        return EXIT_FAILURE;
    }

    /* Пытаемся отобразить разделяемую память в адресное пространство текущего процесса */ 
    shared_memory = shmat(shared_memory_id, NULL, 0);
    if(shared_memory == (void*)(-1))
    {
        perror("Cant attach shared memory");
    
        return EXIT_FAILURE;
    }

    /* Печатаем содержимое разделяемой памяти */
    printf ("%s\n", shared_memory);

    /* Отсоединяем разделяемую память и завершаем работу */ 
    int shmdt_res = shmdt(shared_memory);
    if(shmdt_res < 0)
    {
        perror("Error in shmdt");

        return EXIT_FAILURE;
    }

    /* Удаляем разделяемую память из системы */ 
    (void)shmctl(shared_memory_id, IPC_RMID, (struct shmid_ds *)NULL);

    return 0;
}
