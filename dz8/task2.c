//написать программу получающую в качестве параметров два номера. Первый номер это 
//номер для получения сообщений, второй номер для передачи сообщений. Можно  
//запустить несколько экземпляров такой программы и они все будут обмениваться  
//сообщениями между собой по номерам.
// Всё это нужно реализовать через одну очередь. 

// идея: сообщения имеют тип -> каждому номеру можно присвоить тип

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define LAST_MESSAGE 255
#define MAX_MSG_SIZE 255
const char kSyncFile[]="queue_meow";
const int kFullAccess = 0666;

typedef enum TerminateStatus {
    TerminateStatus_kOk            = 0,
    TerminateStatus_kInternalError = 1,
} TerminateStatus;

TerminateStatus SendMsg(int queue_id, long write_queue_type);
TerminateStatus ReceiveMsg(int queue_id, long read_queue_type);

int main(const int argc, const char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Not enough args\n");

        return EXIT_FAILURE;
    }
    
    long read_queue_type = strtol(argv[1], NULL, /* base */ 10);
    long write_queue_type = strtol(argv[2], NULL, /* base */ 10);
    
    key_t key = ftok(kSyncFile, 0);
    // Если очередь уже существует, то ничего не делаем  
    int msqid = msgget(key, kFullAccess | IPC_CREAT);
    if (msqid < 0) {
        perror("Cant open queue");

        return EXIT_FAILURE;
    }
   
    pid_t new_pid = fork();
    if (new_pid == 0) { // child
        ReceiveMsg(msqid, read_queue_type);
    } else { // parent
        SendMsg(msqid, write_queue_type);
    }

    if (new_pid != 0) {
        int proc_status = 0;
        waitpid(new_pid, &proc_status, 0);
        assert(WIFEXITED(proc_status));
    }
}

    // int len, maxlen;

typedef struct MsgBuffer {
    long type;
    char text[MAX_MSG_SIZE + 1];
} MsgBuffer;

TerminateStatus SendMsg(int queue_id, long write_queue_type) {
    // второй процесс ожидает ввода с клавиатуры и отправляет сообщения,
    // встретив символ перевода строки 

    MsgBuffer msg_buf = {.type = write_queue_type};
    while (/*fputs("Send: ", stdout),*/ fgets(msg_buf.text, MAX_MSG_SIZE, stdin) != NULL) {
        size_t msg_len = strlen(msg_buf.text) + 1;
        int send_res = msgsnd(queue_id, &msg_buf, msg_len, 0);
        if (send_res < 0) {
            perror("SendMsg: Cant send message");

            return TerminateStatus_kInternalError;
        }
    }

    int send_res = msgsnd(queue_id, &msg_buf, 0, 0);
    if (send_res < 0) {
        perror("SendMsg: Cant send last message");

        return TerminateStatus_kInternalError;
    }

    fprintf(stderr, "SendMsg: exited\n");
    return TerminateStatus_kOk;
}

TerminateStatus ReceiveMsg(int queue_id, long read_queue_type) {
    // один из процессов проверяет очередь на предмет наличия в ней сообщений,
    // адресованных данному терминалу и выводит их на экран
    
    MsgBuffer msg_buf = {.type = read_queue_type};
    ssize_t rec_res = 0;
    while ((rec_res = msgrcv(queue_id, &msg_buf, MAX_MSG_SIZE + 1, read_queue_type, 0)) > 0) {
        if (rec_res == 0) {
            return TerminateStatus_kOk;
        } else if (rec_res < 0) {
            perror("ReceiveMsg: cant receive message");

            return TerminateStatus_kInternalError;
        }

        // fputs("\n", stdout);
        fputs("Receive: ", stdout);        
        fputs(msg_buf.text, stdout);
    }

    fprintf(stderr, "ReceiveMsg: exited\n");
    return TerminateStatus_kOk;
}

