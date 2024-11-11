#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"

const size_t kShmemSize = 4096;
const char kMessage[] = "Hello World!\n";
const char kSyncFile[] = "sync_file";

const int kUserReadWriteAccess = 0600;

// Написать комментарии, отладить работу
// NOTE Исопользоваьь вместе с task4.c

typedef union semnum 
{
	int val;
	struct semid_ds* buf;
	unsigned short* array;
} semnum;


void DumpSemaphoreState_(const int sem_id, const char* file, const int line, const char* func);
#define DumpSemaphoreState(sem_id) DumpSemaphoreState_(sem_id, __FILE__, __LINE__, __func__)

int main(void) {
    DEBUG_PRINT("Pid: %d\n", getpid());

    key_t data_key = ftok(kSyncFile, 0);
    if (data_key == (key_t)(-1)) {
        perror("Error ftok for data file");

        return EXIT_FAILURE;
    }

    DEBUG_PRINT("data_key: %x\n", (unsigned)data_key);

	int shm_id = shmget(data_key, kShmemSize, kUserReadWriteAccess | IPC_CREAT /*| IPC_EXCL */);
	if (shm_id < 0) 
	{
		perror("Can get shared memory");

        return EXIT_FAILURE;
	}

	int sem_id = semget(data_key, 
                        1, /* колво семафор */ 
                        kUserReadWriteAccess 
                            | IPC_CREAT /* если нет в система - создаьб*/ 
                            /*| IPC_EXCL*/ /* если уже существует то вызовет ошибку */);
	if (sem_id < 0)
	{
		perror("Can get semaphore");

        return EXIT_FAILURE;
	}

    DEBUG_PRINT("Semaphore: %d\n", sem_id);	
    DEBUG_PRINT("Shared mem: %d\n", shm_id);

	unsigned short sem_vals = 0;
    int ctl_res = semctl(sem_id, 0 /* игнорируется */, SETALL, (semnum){.array = &sem_vals});
	if (ctl_res < 0) {
		perror("semctl() error");

        return EXIT_FAILURE;
	}

	char* shm_buf = (char*)shmat(shm_id, NULL, 0); // map memory to address space of the process
	if (shm_buf == (char*)-1)
	{
		perror("shmat() error");
		
        return EXIT_FAILURE;
	}
    
    struct shmid_ds shmem_stat;
	shmctl(shm_id, IPC_STAT, &shmem_stat);
	
	int shm_size = shmem_stat.shm_segsz;
	if (shm_size < strlen(kMessage))
	{
		fprintf(stderr, "error: segsize=%d\n", shm_size);

        return EXIT_FAILURE;
	}
	
	strcpy(shm_buf, kMessage);

    struct sembuf sem_cmd = {
        .sem_num = 0,
        // .sem_op
        .sem_flg = SEM_UNDO
    };	

    sem_cmd.sem_op = -1;
	int semop_res = semop(sem_id, &sem_cmd, 1);
    if (semop_res < 0) {
        perror("Cant sub 1 from semaphore");

        goto clear_resources;
    }

	semop_res = sem_cmd.sem_op = 0;
	semop(sem_id, &sem_cmd, 1);
    if (semop_res < 0) {
        perror("Cant add 1 to semaphore");

        goto clear_resources;
    }

clear_resources:
	semctl(sem_id, 0 /* ignored */, IPC_RMID, NULL); // remove semaphore from system
	
    shmdt(shm_buf); // detaches shared memory from address space
	shmctl(shm_id, IPC_RMID, NULL);

	return 0;
}

void DumpSemaphoreState_(const int sem_id, const char* file, const int line, const char* func) {
    assert(file != NULL);
    assert(line > 0);
    assert(func != NULL);

#define DEBUG
#if defined (DEBUG)

    fprintf(stderr, ">>> dump semaphore state: %s:%d\n", file, line);

    pid_t last_access_pid = semctl(sem_id, 0, GETPID);
    fprintf(stderr, ">>> last access pid: %d\n", last_access_pid);

    int sem_value = semctl(sem_id, 0, GETVAL);
    fprintf(stderr, ">>> semaphore value: %d\n", sem_value);
#endif // DEBUG
}
