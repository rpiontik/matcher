/*
 * trtransmit.h
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_WORKERS_TRTRANSMIT_H_
#define MODULES_WORKERS_TRTRANSMIT_H_

#include <yaml.h>
#include <stdint.h>
#include "../../libs/hiredis/hiredis.h"
#include "../../consts.h"
#include "../markets/transaction.h"

struct trtr_context_t {
	char logdir[256];
	//char redis_server[256];
	//int redis_port;
};

struct trtr_pipe_t {
	int pipe[2];
	uint64_t transmited_to_steams;
	pthread_mutex_t mutex;
	FILE * streams[MARKETS_NUMBER];
	redisContext * redis;
};

extern struct trtr_pipe_t trtr_pipe;

void trTransmiterInit(void);
void * trtrContextConstructor(yaml_document_t * document, yaml_node_t * node);
void * trtrProccess(void * context);
void trtrTransmitTransaction(struct transaction_t * transaction);


#endif /* MODULES_WORKERS_TRTRANSMIT_H_ */
