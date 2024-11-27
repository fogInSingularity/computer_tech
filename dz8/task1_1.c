#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LAST_MESSAGE 255
#define MAX_MSG_SIZE 81
const char kSyncFile[]="queue_meow";
const int kFullAccess = 0666;

int main(void) { 
    struct mymsgbuf {
        long mtype;
        char mtext[MAX_MSG_SIZE];
    } mybuf;

    /* Create or attach message queue  */
    key_t key = ftok(kSyncFile, 0);
    int msqid = msgget(key, kFullAccess | IPC_CREAT);
    if (msqid < 0) {
        perror("Can\'t get msqid");

        return EXIT_FAILURE;
    }

    /* Send information */
    for (int i = 1; i <= 5; i++) {
        mybuf.mtype = 1;
        strcpy(mybuf.mtext, "This is text message");
        size_t len = strlen(mybuf.mtext) + 1;
        
        int send_res = msgsnd(msqid, (struct msgbuf*)&mybuf, len, 0 /* без флагов */);
        if (send_res < 0) {
            perror("Can\'t send message to queue");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            
            return EXIT_FAILURE;
        }
    }    
       
    /* Send the last message */   
    mybuf.mtype = LAST_MESSAGE;   

    int send_res = msgsnd(msqid, (struct msgbuf *) &mybuf, /* len */ 0, 0);
    if (send_res < 0) {
        perror("Can\'t send message to queue");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        
        return EXIT_FAILURE;
    }

    return 0; 
}
