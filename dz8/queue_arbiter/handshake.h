#ifndef HANDSHAKE_H_
#define HANDSHAKE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "cfg.h"

static const char kQueueFile[] = "arbiter_queue";
static const char kShmemFile[] = "arbiter_shmem";
static const int kFullAccess = 0666;

static const long kArbiterId = 1;

typedef enum CallStatus {
    CallStatus_kOk = 0,
    CallStatus_kCantRegister = 1,
} CallStatus;

typedef enum AMessagePolicy {
    MessageArbiterPolicy_kRequestDestId = 0,
    MessageArbiterPolicy_kRegisterClient = 1,
    
} AMessagePolicy;

typedef struct AMessage {
    long type;
    AMessagePolicy policy;
    union {
        struct {
            pid_t client_pid; // use for response 
            char client_name[MAX_CLIENT_NAME_LEN];    
        } register_client;
        struct {
            long who_ask_id;
            char dest_name[MAX_CLIENT_NAME_LEN];
        } request_dest_id;
    } mes_content;
} AMessage;

typedef enum AResponsePolicy {
    AResponsePolicy_kArbiterUninit         = 0,
    AResponsePolicy_kArbiterHasNoResponses = 1,
    AResponsePolicy_kRegisterClient        = 2,
    // AResponsePolicy_kRequestDestId  = 2,
} AResponsePolicy;

typedef struct AResponse {
    pthread_mutex_t resp_mutex;
    pthread_cond_t resp_cond;
    AResponsePolicy resp_policy;
    struct {
        pid_t resp_pid; // used for register name
        long resp_id_for_arbiter;
        long resp_id_for_rmessages;
    } resp_content;
    bool client_resp;
} AResponse;

typedef enum RMessagePolicy {
    RMessagePolicy_kRespondWithDestId = 0,
    RMessagePolicy_kRegularMessage = 1,
} RMessagePolicy;
// FIXME
typedef struct RegularMessage {
    long type;
    RMessagePolicy policy;
    union {
        long dest_id;
        char mes[MAX_MESSAGE_SIZE];
    } mes_content;
} RegularMessage;

#endif // HANDSHAKE_H_
