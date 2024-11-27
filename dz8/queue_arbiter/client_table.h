#ifndef CLIENT_TABLE_H_
#define CLIENT_TABLE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "cfg.h"

typedef struct ClientInfo {
    char client_name[MAX_CLIENT_NAME_LEN];
    long id_for_arbiter;
    long id_for_rmessages;
    bool is_awaken;
} ClientInfo;

typedef struct ClientTable {
    pthread_mutex_t table_mutex;
    long free_id;
    ClientInfo client_info[MAX_N_CLIENTS];
} ClientTable;

void InitTable(ClientTable* table);

void RemoveNotAwaken(ClientTable* table);
bool AddToTable(ClientTable* table, const char new_client_name[], long* new_id_for_arbiter, long* new_id_for_rmessages);
long GetIdForArbiterFromName(ClientTable* table, const char client_name[]);
long GetIdForRMessagesFromName(ClientTable* table, const char client_name[]);

#endif // CLIENT_TABLE_H_
