#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <pthread.h>

#include "log.h"

#include "handshake.h"
#include "cfg.h"
#include "client_table.h"

#define ARBITER_PROMPT "Arbiter: "

typedef struct Arbiter {
    ClientTable client_table;
    int queue_id;
    // int shmem_id;
    AResponse* response;
} Arbiter;

void FreeIPC(int queue_id, int shmem_id, void* shmem_ptr);
void* DisconectService(void* arg);
void ListenOnMessages(Arbiter* arbiter);

void RegisterClient(Arbiter* arbiter, AMessage* mes);
void SendDestId(Arbiter* arbiter, AMessage* mes);

int main(const int argc, const char* argv[argc + 1]) {
    LoggingStatus log_status = LoggingSetup("log_arbiter.log");
    assert(log_status == kLoggingStatus_Ok);

    int queue_id = msgget(ftok(kQueueFile, 0), kFullAccess | IPC_CREAT);
    if (queue_id < 0) {
        perror(ARBITER_PROMPT "cant get queue id");
        Log("cant get queue\n");
        FreeIPC(queue_id, -1, (void*)-1);

        return EXIT_FAILURE;
    }
    LogVariable("%d", queue_id);

    int shmem_id = shmget(ftok(kShmemFile, 0), sizeof(AResponse), kFullAccess | IPC_CREAT);
    if (shmem_id < 0) {
        perror(ARBITER_PROMPT "cant get shmem id");
        Log("cant get shmem\n");
        FreeIPC(queue_id, shmem_id, (void*)-1);

        return EXIT_FAILURE;
    }
    LogVariable("%d", shmem_id);

    void* shmem_ptr = shmat(shmem_id, NULL, 0);
    if (shmem_ptr == (void*)-1) {
        perror(ARBITER_PROMPT "cant attach shmem");
        Log("cant attach shmem\n");
        FreeIPC(queue_id, shmem_id, shmem_ptr);

        return EXIT_FAILURE;
    }
    LogVariable("%p", shmem_ptr);

    AResponse* response = (AResponse*)shmem_ptr;

    Log("init response");
    // init responese
    response->resp_policy = AResponsePolicy_kArbiterHasNoResponses;

    pthread_mutexattr_t mutex_attr = {0};
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&response->resp_mutex, &mutex_attr); // NOTE might be not NULL
    
    pthread_condattr_t cond_attr = {0}; 
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);

    pthread_cond_init(&response->resp_cond, &cond_attr);
    response->resp_content.resp_id_for_arbiter = 0;
    response->resp_content.resp_id_for_rmessages = 0;
    response->resp_content.resp_pid = 0;
    response->client_resp = false;

    Arbiter arbiter = {
        .client_table = {0},
        .queue_id = queue_id,
        .response = response,
    };

    InitTable(&arbiter.client_table);
    Log("response inited\n");

    // pthread_t disconect_service_thread = {0};    
    // pthread_create(&disconect_service_thread, NULL, DisconectService, &arbiter);
    
    ListenOnMessages(&arbiter);

    // pthread_cancel(disconect_service_thread);
    // pthread_join(disconect_service_thread, NULL);

    FreeIPC(queue_id, shmem_id, shmem_ptr);

    return 0;
}

void FreeIPC(int queue_id, int shmem_id, void* shmem_ptr) {
    LogFunctionEntry();

    if (shmem_ptr != (void*)-1) {
        shmdt(shmem_ptr);
    }

    if (shmem_id != -1) {
        shmctl(shmem_id, IPC_RMID, NULL);
    }

    if (queue_id != -1) {
        msgctl(queue_id, IPC_RMID, NULL);
    }
}

void* DisconectService(void* arg) {
    assert(arg != NULL);

    LogFunctionEntry();

    Arbiter* arbiter = arg;

    // FIXME  
    
    return NULL;
}

void ListenOnMessages(Arbiter* arbiter) {
    assert(arbiter != NULL);

    LogFunctionEntry();

    AMessage mes = {0}; 

    ssize_t rcv_res = 0;
    while ((rcv_res = msgrcv(arbiter->queue_id, &mes, sizeof(mes) - sizeof(mes.type), kArbiterId, 0)) >= 0) {
        // LogVariable("%ld", rcv_res);

        switch (mes.policy) {
            case MessageArbiterPolicy_kRegisterClient:
                RegisterClient(arbiter, &mes);
                break;
            case MessageArbiterPolicy_kRequestDestId:
                SendDestId(arbiter, &mes);
                break;
            default:
                assert(0 && "unexpected policy");                
                break;
        }
    }
}

void RegisterClient(Arbiter* arbiter, AMessage* mes) {
    assert(arbiter != NULL);
    assert(mes != NULL);

    LogFunctionEntry();

    // NOTE maybe use under lower mutex
    long new_id_for_arbiter = 0;
    long new_id_for_rmessages = 0;
    LogVariable("%s", mes->mes_content.register_client.client_name);
    bool is_added = AddToTable(&arbiter->client_table, mes->mes_content.register_client.client_name, &new_id_for_arbiter, &new_id_for_rmessages);
    assert(is_added);

    Log("client where added to table\n");

    Log("try to take mutex lock\n");
    pthread_mutex_lock(&arbiter->response->resp_mutex);
    Log("mutex lock acquired\n");

    arbiter->response->resp_policy = AResponsePolicy_kRegisterClient;

    arbiter->response->resp_content.resp_pid = mes->mes_content.register_client.client_pid;
    arbiter->response->resp_content.resp_id_for_arbiter = new_id_for_arbiter;
    arbiter->response->resp_content.resp_id_for_rmessages = new_id_for_rmessages;

    pthread_mutex_unlock(&arbiter->response->resp_mutex);
    Log("mutex unlocked\n");

    Log("broadcast\n");
    pthread_cond_broadcast(&arbiter->response->resp_cond);

    Log("wait on client_resp\n");
    while (!arbiter->response->client_resp) {} // NOTE spin lock?
    Log("wait ended, client responded\n");
 
    Log("try to take mutex lock\n");
    pthread_mutex_lock(&arbiter->response->resp_mutex);
    Log("mutex lock acquired\n");

    arbiter->response->client_resp = false;
    arbiter->response->resp_policy = AResponsePolicy_kArbiterHasNoResponses;
    arbiter->response->resp_content.resp_pid = 0;
    arbiter->response->resp_content.resp_id_for_arbiter = 0;
    arbiter->response->resp_content.resp_id_for_rmessages = 0;

    pthread_mutex_unlock(&arbiter->response->resp_mutex);
    Log("mutex unlocked\n");
}

void SendDestId(Arbiter* arbiter, AMessage* mes) {
    assert(arbiter != NULL);
    assert(mes != NULL);

    LogFunctionEntry();
    
    LogVariable("%s", mes->mes_content.request_dest_id.dest_name);
    long dest_id = GetIdForRMessagesFromName(&arbiter->client_table, mes->mes_content.request_dest_id.dest_name);
    LogVariable("%ld", dest_id);

    RegularMessage resp_mes = {0};
    if (dest_id >= 0) {
        resp_mes = (RegularMessage){
            .type = mes->mes_content.request_dest_id.who_ask_id, 
            .policy = RMessagePolicy_kRespondWithDestId,
            .mes_content.dest_id = dest_id,
        };
    } else {
        resp_mes = (RegularMessage){
            .type = mes->mes_content.request_dest_id.who_ask_id,
            .policy = RMessagePolicy_kRespondWithDestId,
            .mes_content.dest_id = -1
        };
    }

    size_t resp_mes_size = sizeof(resp_mes) - sizeof(resp_mes.type);
    int send_res = msgsnd(arbiter->queue_id, &resp_mes, resp_mes_size, 0);
    LogVariable("%d", send_res);
}
