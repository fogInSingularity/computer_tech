#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/sem.h>

#include "debug.h"

const size_t kShmemSize = 4096;
const char kSyncFile[] = "sync_file";

const int kUserReadWriteAccess = 0600;

// Написать комментарии, отладить работу

int main (const int argc, const char* argv[]) {
    key_t data_key = ftok(kSyncFile, 0);
    if (data_key == (key_t)(-1)) {
        perror("Error ftok for data file");

        return EXIT_FAILURE;
    }

    int shm_id = shmget(data_key, kShmemSize, kUserReadWriteAccess);
	if (shm_id == -1) {
		perror("shmget() error");

        return EXIT_FAILURE;
	}
	
	int sem_id = semget(data_key, 1, kUserReadWriteAccess);
	if (sem_id == -1) {
		perror("semget() error");

		return EXIT_FAILURE;
	}

    char* shm_buf = (char*)shmat(shm_id, NULL, 0);
	if (shm_buf == (char *) -1) {
		perror("shmat() error");

        return EXIT_FAILURE;
	}

	printf("Message: %s\n", shm_buf);

	int semop_res = semop(sem_id, &(struct sembuf){.sem_num = 0, .sem_op = 1, .sem_flg = SEM_UNDO}, 1);
	if (semop_res < 0) {
        perror("Cant sub 1 from semaphore");
    }

	shmdt (shm_buf);

	return 0;
}
