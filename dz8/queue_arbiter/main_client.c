#include <pthread.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "log.h"

#include "handshake.h"
#include "cfg.h"

#define CLINENT_PROMPT "Client: "

const char kHelp[] = "usage: ./client <name>\n";

typedef struct Client {
    int queue_id;
    long id_for_arbiter;
    long id_for_rmessages;
} Client;

void* ReaderThreadTask(void* reader_arg);
void* SenderThreadTask(void* sender_arg);

CallStatus RegisterInArbiter(Client* client, AResponse* response, const char* client_name);

ssize_t ParseMessage(const char user_input[], char dest_name[], char dest_mes[]);
long GetReceiverId(const Client* const client, const char client_name[]);
void SendToReceiver(const Client* const client, const char mes[], long dest_id);

// client should give its name as argv
int main(const int argc, const char* argv[argc + 1]) {
    if (argc < 2) {
        fputs(kHelp, stderr);

        return EXIT_FAILURE;
    }

    if (strlen(argv[1]) > MAX_CLIENT_NAME_LEN - 1) {
        fputs("Too long name", stderr);

        return EXIT_FAILURE;
    }

    char generate_log_file_name[MAX_CLIENT_NAME_LEN * 2] = {0};
    snprintf(generate_log_file_name, MAX_CLIENT_NAME_LEN * 2, "log_%s.log", argv[1]);
    LoggingStatus log_status = LoggingSetup(generate_log_file_name);
    assert(log_status == kLoggingStatus_Ok);

    int queue_id = msgget(ftok(kQueueFile, 0), kFullAccess);
    if (queue_id < 0) {
        perror(CLINENT_PROMPT "cant get queue id");
        Log("cant get queue\n");

        return EXIT_FAILURE;
    }
    LogVariable("%d", queue_id);


    int shmem_id = shmget(ftok(kShmemFile, 0), sizeof(AResponse), kFullAccess);
    if (shmem_id < 0) {
        perror(CLINENT_PROMPT "cant get shmem id");
        Log("cant get shmem\n");

        return EXIT_FAILURE;
    }
    LogVariable("%d", shmem_id);

    void* shmem_region = shmat(shmem_id, NULL, 0);
    if (shmem_region == (void*)-1) {
        perror(CLINENT_PROMPT "cant attach shmem");
        Log("cant attach shmem\n");

        return EXIT_FAILURE;
    }
    LogVariable("%p", shmem_region);
    
    AResponse* response = shmem_region;

    Client client = {
        .queue_id = queue_id,
        .id_for_arbiter = 0,
        .id_for_rmessages = 0,
    };

    Log("try to register\n");
    CallStatus status = RegisterInArbiter(&client, response, argv[1]);
    if (status == CallStatus_kCantRegister) {
        fprintf(stderr, CLINENT_PROMPT "cant register %s\n", argv[1]);
        Log("cant register(fuuuck)\n");

        return EXIT_FAILURE;
    }
    Log("registered succesfully\n");

    // async read and send mes

    // 1) thread for sending messages
    // 2) thread for receiving messages

    pthread_t reader_thread = {0};
    pthread_t sender_thread = {0};

    pthread_create(&reader_thread, NULL, ReaderThreadTask, &client);
    pthread_create(&sender_thread, NULL, SenderThreadTask, &client);

    pthread_join(reader_thread, NULL);
    pthread_join(sender_thread, NULL);

    fprintf(stderr, CLINENT_PROMPT "exits\n");

    return 0;
}

void* ReaderThreadTask(void* reader_arg) {
    assert(reader_arg != NULL);

    LogFunctionEntry();

    const Client* const client = reader_arg;

    RegularMessage reg_mes = {0};
    int rcv_res = 0;

    while ((rcv_res = msgrcv(client->queue_id, &reg_mes, sizeof(reg_mes) - sizeof(reg_mes.type), client->id_for_rmessages, 0))) {
        assert(rcv_res >= 0);

        // FIXME exit?

        fputs(reg_mes.mes_content.mes, stdout);
    }

    return NULL;
}

void* SenderThreadTask(void* sender_arg) {
    assert(sender_arg != NULL);
 
    LogFunctionEntry();

    const Client* const client = sender_arg;

    char user_input_buffer[MAX_CLIENT_NAME_LEN + MAX_MESSAGE_SIZE] = {0};
    while (fgets(user_input_buffer, MAX_MESSAGE_SIZE, stdin) != NULL) {
        char dest_name[MAX_CLIENT_NAME_LEN] = {0};
        char dest_mes[MAX_MESSAGE_SIZE] = {0};
        ParseMessage(user_input_buffer, dest_name, dest_mes);
        long dest_id = GetReceiverId(client, dest_name);
        SendToReceiver(client, dest_mes, dest_id);
    }
    
    return NULL;
}

// format: name#message
// returns dest_name len or -1 on error
ssize_t ParseMessage(const char user_input[], char dest_name[], char dest_mes[]) {
    assert(user_input != NULL);
    assert(dest_name != NULL);
    assert(dest_mes != NULL);

    LogFunctionEntry();

    size_t user_input_len = strlen(user_input);

    const char* separator = strchr(user_input, '#');
    if (separator == NULL) { return -1; }

    size_t name_len = separator - user_input;
    size_t mes_len = user_input_len - name_len - 1;

    memcpy(dest_name, user_input, name_len);
    dest_name[name_len] = '\0';

    memcpy(dest_mes, separator + 1, mes_len);
    dest_mes[mes_len] = '\0';
    
    return name_len;
}

CallStatus RegisterInArbiter(Client* client, AResponse* response, const char* client_name) {
    assert(client != NULL);
    assert(response != NULL);
    assert(client_name != NULL);

    LogFunctionEntry();

    AMessage msg = {
        .type = kArbiterId,
        .policy = MessageArbiterPolicy_kRegisterClient,       
        .mes_content.register_client.client_pid = getpid(),
        .mes_content.register_client.client_name = {0},
    };

    strncpy(msg.mes_content.register_client.client_name, client_name, MAX_CLIENT_NAME_LEN);

    Log("send request(message)\n");
    ssize_t send_res = msgsnd(client->queue_id, &msg, sizeof(msg) - sizeof(msg.type), 0);
    if (send_res < 0) {
        perror(CLINENT_PROMPT "cant send register request");

        return CallStatus_kCantRegister;
    }

    // FIXME check if inited

    bool responded_to_me = false;

    Log("try to lock mutex\n");
    pthread_mutex_lock(&response->resp_mutex);
    Log("mutex locked\n");
    while (!responded_to_me) {
        Log("take cond wait\n");
        pthread_cond_wait(&response->resp_cond, &response->resp_mutex);
        Log("awaken from cond\n");
        if (response->resp_content.resp_pid == getpid()) {
            Log("my pid encontered\n");
            responded_to_me = true;
            client->id_for_arbiter = response->resp_content.resp_id_for_arbiter;
            client->id_for_rmessages = response->resp_content.resp_id_for_rmessages;
        }
    }
    pthread_mutex_unlock(&response->resp_mutex);
    Log("mutex unlocked\n");

    response->client_resp = true;
    
    Log("end of register\n");

    LogVariable("%ld", client->id_for_arbiter);
    LogVariable("%ld", client->id_for_rmessages);

    return CallStatus_kOk;
}

long GetReceiverId(const Client* const client, const char client_name[]) {
    assert(client != NULL);
    assert(client_name != NULL);

    LogFunctionEntry();

    AMessage mes = {
        .type = kArbiterId,
        .policy = MessageArbiterPolicy_kRequestDestId,
        .mes_content.request_dest_id.who_ask_id = client->id_for_arbiter,
        .mes_content.request_dest_id.dest_name = {0}
    };
    strncpy(mes.mes_content.request_dest_id.dest_name, client_name, MAX_CLIENT_NAME_LEN);

    int send_res = msgsnd(client->queue_id, &mes, sizeof(mes) - sizeof(mes.type), 0);
    // FIXME check send_res
    RegularMessage rcv_mes = {0};
    // FIXME if at this time regular message coms in
    int rcv_res = msgrcv(client->queue_id, &rcv_mes, sizeof(rcv_mes) - sizeof(rcv_mes.type), client->id_for_arbiter, 0);
    // FIXME check

    LogVariable("%ld", rcv_mes.mes_content.dest_id);
    return rcv_mes.mes_content.dest_id;
}

void SendToReceiver(const Client* const client, const char mes[], long dest_id) {
    assert(client != NULL);
    assert(mes != NULL);

    LogFunctionEntry();
    
    RegularMessage mes_to_rec = {
        .type = dest_id,
        .policy = RMessagePolicy_kRegularMessage,
        .mes_content.mes = {0}
    };
    strcpy(mes_to_rec.mes_content.mes, mes);

    int send_res = msgsnd(client->queue_id, &mes_to_rec, sizeof(mes_to_rec) - sizeof(mes_to_rec.type), 0);
    // FIXME check
}













