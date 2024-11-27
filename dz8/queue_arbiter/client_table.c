#include "client_table.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "cfg.h"
#include "handshake.h"
#include "log.h"

void InitTable(ClientTable* table) {
    assert(table != NULL);

    LogFunctionEntry();

    memset(table->client_info, 0, sizeof(table->client_info));
    table->free_id = kArbiterId + 1;
    pthread_mutex_init(&table->table_mutex, NULL);
}

void RemoveNotAwaken(ClientTable* table) {
    assert(table != NULL);

    pthread_mutex_lock(&table->table_mutex);

    for (size_t i = 0; i < MAX_N_CLIENTS; i++) {
        if ((table->client_info[i].id_for_arbiter != 0) && !table->client_info[i].is_awaken) {
            memset(&table->client_info[i], 0, sizeof(table->client_info[i]));
        }
    }

    pthread_mutex_unlock(&table->table_mutex);
}

bool AddToTable(ClientTable* table, const char new_client_name[], long* new_id_for_arbiter, long* new_id_for_rmessages) {
    assert(table != NULL);
    assert(new_client_name != NULL);

    LogFunctionEntry();
    LogVariable("%s", new_client_name);

    pthread_mutex_lock(&table->table_mutex);

    ssize_t available_slot = -1;
    for (size_t i = 0; i < MAX_N_CLIENTS; i++) {
        if (table->client_info[i].id_for_arbiter == 0) {
            available_slot = i;
            break;
        }
    }
    
    if (available_slot == -1) {
        Log("cant add to table");
        return false;
    }

    long added_id_for_arbiter = table->free_id;
    table->free_id++;
    long added_id_for_rmessages = table->free_id;
    table->free_id++;

    table->client_info[available_slot].id_for_arbiter = added_id_for_arbiter;
    table->client_info[available_slot].id_for_rmessages = added_id_for_rmessages;
    table->client_info[available_slot].is_awaken = true;
    strncpy(table->client_info[available_slot].client_name, new_client_name, MAX_CLIENT_NAME_LEN);

    pthread_mutex_unlock(&table->table_mutex);

    *new_id_for_arbiter = added_id_for_arbiter;
    *new_id_for_rmessages = added_id_for_rmessages;

    return true;
}

long GetIdForArbiterFromName(ClientTable* table, const char client_name[]) {
    assert(table != NULL);
    assert(client_name != NULL);

    for (size_t i = 0; i < MAX_N_CLIENTS; i++) {
        if (!strcmp(client_name, table->client_info[i].client_name)) {
            return table->client_info[i].id_for_arbiter;
        }
    }

    return -1; // not found
}

long GetIdForRMessagesFromName(ClientTable* table, const char client_name[]) {
    assert(table != NULL);
    assert(client_name != NULL);

    for (size_t i = 0; i < MAX_N_CLIENTS; i++) {
        if (!strcmp(client_name, table->client_info[i].client_name)) {
            return table->client_info[i].id_for_rmessages;
        }
    }

    return -1; // not found
}
