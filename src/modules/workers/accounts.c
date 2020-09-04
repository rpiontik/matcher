/*
 * accounts.c
 *
 *  Created on: 15 авг. 2018 г.
 *      Author: rpiontik
 */

#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <yaml.h>
#include "accounts.h"
#include "depthtree.h"
#include "../utils.h"
#include "../types.h"

pthread_mutex_t accounts_mutex = {0};
struct accounts_table_t * accounts = NULL;

inline void accountsLock(void) {
	pthread_mutex_lock(&accounts_mutex);
}

inline void accountsUnlock(void) {
	pthread_mutex_unlock(&accounts_mutex);
}

static inline uint64_t makeHash(uint32_t id) {
	return ((~(uint64_t)id) << 32) | (uint32_t)id;
}

struct account_t * findAccount(uint32_t id) {
	if(accounts == NULL)
		return NULL;
	intptr_t index = (intptr_t)findBTreeLocation(accounts->btree, id);
	return !index ? NULL : &accounts->accounts[index - 1];
}

void appendAccountsBlock(void){
	if(accounts == NULL) {
		accounts = malloc(sizeof(struct accounts_table_t) + sizeof(struct account_t) * DEF_ACCOUNTS_MEM_BLOCK_RECS);
		accounts->allocated = DEF_ACCOUNTS_MEM_BLOCK_RECS;
		accounts->btree = createBTreeNode();
		accounts->count = 0;
	} else {
		accounts = realloc(accounts, sizeof(struct accounts_table_t) + sizeof(struct account_t)
				* (accounts->allocated + DEF_ACCOUNTS_MEM_BLOCK_RECS));
		accounts->allocated += DEF_ACCOUNTS_MEM_BLOCK_RECS;
	}
	memset(&accounts->accounts[accounts->count], 0, sizeof(struct account_t) * DEF_ACCOUNTS_MEM_BLOCK_RECS);
}

struct account_t * appendAccount(uint32_t id) {
	if((accounts == NULL) || (accounts->count >= accounts->allocated))
		appendAccountsBlock();
	struct account_t * account = &accounts->accounts[accounts->count++];
	account->id = id;
	appendBTreeKey(accounts->btree, id, (void*)(intptr_t)accounts->count);
	return account;
}

void accountsMakeDump(struct account_context_t * context, uint64_t now) {
	accountsLock();
	int dump_size = sizeof(struct accounts_table_t) + sizeof(struct account_t) * accounts->allocated;
	struct account_dump_t * dump = malloc(dump_size + sizeof(struct account_dump_t));
	dump->last_transaction = transaction_counter.counter;
	memcpy(&dump->accounts_table, accounts, dump_size);
	accountsUnlock();

	dump_size += sizeof(struct account_dump_t);
	dump->crc = crc32(&dump->last_transaction, dump_size - sizeof(dump->crc), 0);

	char dum_file_name[1024] = {0};
	sprintf(dum_file_name, "%s\%lu.accounts", context->dump_path, now);
	FILE * dump_file = fopen(dum_file_name, "w");

	if(dump_file == NULL) {
		CRITICAL_ERROR("Critical error: can not write dump account to file [%s]", dum_file_name);
	} else {
		fwrite(dump, 1, dump_size, dump_file);
		fclose(dump_file);
	}

	free(dump);
}

void * accountDumpProccess(void * ctx) {
	struct account_context_t * context = (struct account_context_t *) ctx;
	uint64_t last_moment = now_microseconds();
	uint64_t now = 0;
	while(1) {
		now = now_microseconds();
		if(accounts && (now - last_moment) >= context->dump_interval) {
			accountsMakeDump(ctx, now);
			last_moment = now;
		} else
			usleep(context->dump_interval);
	}
}

void * accountContextConstructor(yaml_document_t * document, yaml_node_t * node) {
	struct account_context_t * context = malloc(sizeof(struct account_context_t));
	memset(context, 0, sizeof(struct account_context_t));

	yaml_node_t * dump_interval	= NULL;
	yaml_node_t * dump_path		= NULL;

	if(node) {
		dump_interval	= yaml_node_get_child(document, node, "dump_interval");
		dump_path		= yaml_node_get_child(document, node, "dump_path");
	}

	strcpy(&context->dump_path[0], dump_path ? yaml_node_get_value(dump_path) : DEF_ACCOUNTS_DUMP_PATH);
	context->dump_interval = dump_interval ? atoi(yaml_node_get_value(dump_interval)) : DEF_ACCOUNTS_DUMP_INTERVAL;

	return context;
}
