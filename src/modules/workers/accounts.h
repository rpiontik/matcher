/*
 * accounts.h
 *
 *  Created on: 15 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_WORKERS_ACCOUNTS_H_
#define MODULES_WORKERS_ACCOUNTS_H_

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <yaml.h>
#include "../types.h"
#include "../../consts.h"
#include "../utils.h"

struct account_t {
	uint32_t id;
	uint64_t balances[MAX_CURR_NUMBER];
	pthread_mutex_t mutex;
};

struct accounts_table_t {
	uint32_t count;
	uint32_t allocated;
	struct btree_node_t * btree;
	struct account_t accounts[0];
};

extern struct accounts_table_t * accounts;

struct account_t * findAccount(uint32_t id);

struct account_context_t {
	int dump_interval;
	char dump_path[255];
};

struct __attribute__((__packed__)) account_dump_t {
	uint32_t crc;
	type_id last_transaction;
	struct accounts_table_t accounts_table;
};

void * accountDumpProccess(void * context);
void * accountContextConstructor(yaml_document_t * document, yaml_node_t * node);

struct account_t * appendAccount(uint32_t id);
struct account_t * findAccount(uint32_t id);

void accountsLock(void);
void accountsUnlock(void);

#endif /* MODULES_WORKERS_ACCOUNTS_H_ */
