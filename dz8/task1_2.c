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
    size_t maxlen;

    struct mymsgbuf {
        long mtype;
        char mtext[MAX_MSG_SIZE];
    } mybuf;
    
    key_t key = ftok(kSyncFile, 0);
    int msqid = msgget(key, kFullAccess | IPC_CREAT);
    if (msqid < 0) {
        perror("Can\'t get msqid");
        return EXIT_FAILURE;
    }
    
    while (1)  {
        maxlen = MAX_MSG_SIZE;
        ssize_t send_len = msgrcv(msqid, 
                                  (struct msgbuf*)&mybuf, 
                                  maxlen, 
                                  0 /* type, принимать как FIFO */, 
                                  0 /* без флагов*/);
        if (send_len  < 0) {
            perror("Can\'t receive message from queue");
            
            return EXIT_FAILURE;
        }

        if (mybuf.mtype == LAST_MESSAGE) {
            msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            
            return 0;
        }
        
        printf("message type = %ld, info = %s\n", mybuf.mtype, mybuf.mtext);
    }    
    
    return 0;       
}
