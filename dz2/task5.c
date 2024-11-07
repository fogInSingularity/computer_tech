/* Add comments to clarify all unclear places.
   1 Increase reliability when writing outgoing message text.
   2 How the unique key for shared memory is generated.
   3 Modify to work with multiple terminals,
    each message should start with the terminal number,
    the terminal number should be passed as a parameter during startup. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

static const key_t kShmKeyBase = (key_t)0xDEADBABE;
static const int kFullAccess = 0777;
static const size_t kPageSize = 4096;
// #define SHM_MAXSIZE 1 << 20
static const int kDelay =  10 * 1000;
static const size_t kOffset = 100;

int main()
{
	printf("--------------------\n"
		   "SHM-Chat 0.1\n"
		   "--------------------\n"
		   "To send a message, just type it and press Enter.\n"
           "Another instance of SHM-Chat receives and displays it.\n"
		   "--------------------\n"
		   "stdin: %s\n"
		   "stdout: %s\n"
		   "--------------------\n"
		   "Enter any number (chat is among instances with the same number): ", 
           ttyname(fileno(stdin)), 
           ttyname(fileno(stdout)));

	key_t shm_key = 0;
	scanf("%d", &shm_key);
	shm_key += kShmKeyBase;

	printf("--------------------\n\n");
	
	// check if more than one instance already running
	int shared_memory_id = shmget(shm_key, kPageSize, kFullAccess | IPC_CREAT);
    if (shared_memory_id < 0) {
        perror("unable to allocate shared memory");
        return EXIT_FAILURE;
    }

	struct shmid_ds shmid_info = {};
	int check_for_error = shmctl(shared_memory_id, IPC_STAT, &shmid_info);
    if (check_for_error < 0) {
        perror("error on shmctl call");
        return EXIT_FAILURE;
    }

	if (shmid_info.shm_nattch > 2 && shmid_info.shm_nattch < 0xFF) {
		fprintf(stderr, "Only one-on-one chat is supported! Now exiting...\n");
		return EXIT_FAILURE;
	}

	pid_t childpid = fork();
	if (childpid == -1) {
		perror("fork error");
        return EXIT_FAILURE;
	}
	
	if (childpid == 0) {// child process: prints incoming messages
		usleep(kDelay);
		int shmid = shmget(shm_key, kPageSize, kFullAccess);
		char* mem = (char*)shmat(shmid, NULL, 0);
		mem[0] = 0;

		while (1) {
			if (mem[0] != 0 && strcmp(mem, ttyname(fileno(stdout))) != 0)
			{
				printf("--> %s\n", mem + kOffset);
				mem[0] = 0;
			}
			usleep(kDelay);
		}
	} else {
		// parent process: scans outgoing messages
		int shmid = shmget(shm_key, kPageSize, IPC_CREAT | kFullAccess);
		char* mem = (char*)shmat(shmid, NULL, 0);
		usleep(kDelay);

		while (1) {
			fgets(mem + kOffset, kPageSize - kOffset, stdin);
			ttyname_r(fileno(stdout), mem, 20);
			usleep(kDelay);
		}
	}
}
