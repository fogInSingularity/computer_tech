#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"

// FIXME не доделано

const size_t kShmemSize = 4096; // page size
const char kSyncFile[] = "sync_file";
const int kUserReadWriteAccess = 0600;

typedef struct CriticalSection {
    int sem_id;
    bool error_occured;
} CriticalSection;

void CriticalSectionCtor(CriticalSection* crit_section, key_t key);
void CriticalSectionDtor(CriticalSection* crit_section);

void CriticalSectionEnter(CriticalSection* crit_section);
void CriticalSectionExit(CriticalSection* crit_section);
void CriticalSectionWait(CriticalSection* crit_section);

int main(void) {
    key_t sync_file_key = ftok(kSyncFile, 0);
    if (sync_file_key == (key_t)-1) {
        perror("can create key");

        return EXIT_FAILURE;
    }
    
    CriticalSection crit_section = {};
    CriticalSectionCtor(&crit_section, sync_file_key);
    if (crit_section.error_occured) { 
        perror("ctor error"); 

        return EXIT_FAILURE; 
    }

    int shmem_id = shmget(sync_file_key, kShmemSize, kUserReadWriteAccess | IPC_CREAT);
    if (shmem_id < 0) {
        perror("can get shared memory");

        return EXIT_FAILURE;
    }

    int* shmem_buf = shmat(shmem_id, NULL, 0);
    if (shmem_buf == (void*)-1) {
        perror("cant attach memory");

        return EXIT_FAILURE;
    }

    pid_t current_pid = getpid();

    $

    bool need_to_exit = false;
    while (!need_to_exit) {
$        CriticalSectionEnter(&crit_section);
    
$        int num = *shmem_buf;
        fprintf(stderr, "[pid: %d] current num value: %d\n", current_pid, num);

        num++;
        *shmem_buf = num;


        $        CriticalSectionExit(&crit_section);
        sleep(1);

        $        CriticalSectionWait(&crit_section); 
$
    }
}

void CriticalSectionCtor(CriticalSection* crit_section, key_t key) {
    assert(crit_section != NULL);

    crit_section->sem_id = 0;
    int sem_id = semget(key, 1, kUserReadWriteAccess | IPC_CREAT);
    if (sem_id < 0) {
        crit_section->error_occured = true;
        return;
    }

	unsigned short sem_vals = 0;
    int ctl_res = semctl(sem_id, 0 /* игнорируется */, SETALL, &sem_vals);
	if (ctl_res < 0) {
        crit_section->error_occured = true;
	} 

    crit_section->sem_id = sem_id;
}

void CriticalSectionDtor(CriticalSection* crit_section) {
    assert(crit_section != NULL);

	semctl(crit_section->sem_id, 0 /* ignored */, IPC_RMID, NULL); // remove semaphore from system
}

void CriticalSectionEnter(CriticalSection* crit_section) {
    assert(crit_section != NULL);

	int semop_res = semop(crit_section->sem_id, &(struct sembuf){.sem_num = 0, .sem_op = -1, .sem_flg = SEM_UNDO}, 1);
	if (semop_res < 0) {
        crit_section->error_occured = true;
    }
}

void CriticalSectionExit(CriticalSection* crit_section) {
    assert(crit_section != NULL);

	int semop_res = semop(crit_section->sem_id, &(struct sembuf){.sem_num = 0, .sem_op = 1, .sem_flg = SEM_UNDO}, 1);
	if (semop_res < 0) {
        crit_section->error_occured = true;
    }    
}

void CriticalSectionWait(CriticalSection* crit_section) {
    assert(crit_section != NULL);

	int semop_res = semop(crit_section->sem_id, &(struct sembuf){.sem_num = 0, .sem_op = 0, .sem_flg = SEM_UNDO}, 1);
	if (semop_res < 0) {
        crit_section->error_occured = true;
    }    
}

