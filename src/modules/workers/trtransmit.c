/*
 * trtransmit.c
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: rpiontik
 */
#include "trtransmit.h"

#include <stdlib.h>
#include <yaml.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

//#include "../../libs/hiredis/hiredis.h"
//#include "../gateways/redis.h"
#include "../markets/transaction.h"
#include "depthtree.h"
#include "../utils.h"
#include "../../consts.h"

struct trtr_pipe_t trtr_pipe = {0};

static void trtrInitStreams(struct trtr_context_t * context) {
	char path[1024] = {0};
	for(int i = 0;  i < MARKETS_NUMBER; i++) {
		sprintf(path, "%s\%d.transactions", context->logdir, i);
		trtr_pipe.streams[i] = fopen(path, "a+");
		if(trtr_pipe.streams[i] == NULL) {
			FATAL_ERROR("\nCould not open or create file [%s]\n", path);
		} else {
			LOGI("\nStarted steam [%s]\n", path);
		}
	}
}

/*
static void trtrInitredis(struct trtr_context_t * context){
	struct timeval timeout = { 1, 500000 };
	trtr_pipe.redis = redisConnectWithTimeout(context->redis_server, context->redis_port, timeout);
	if(trtr_pipe.redis->err) {
		FATAL_ERROR("Connection to Redis error: %s\n", trtr_pipe.redis->errstr);
	} else if (trtr_pipe.redis == NULL) {
		FATAL_ERROR("Connection Redis error: can't allocate redis context\n");
	}
}
*/

void transmitTransaction(struct transaction_t * transaction) {

	if(!(transaction->id % 100000)) {
		printf("\n\n Depth volume to buy: %ju to sell: %ju market: %d stock: %d",
				markets[transaction->market].stocks[transaction->stock].volume_buy,
				markets[transaction->market].stocks[transaction->stock].volume_sell,
				transaction->market,
				transaction->stock
		);
		printf("\n Last transaction time: %d executed: %dpcs last price %ju volume %ju\n\n", transaction->moment, transaction->id, transaction->price, transaction->volume);
	}

	pthread_mutex_lock(&trtr_pipe.mutex);
	write(trtr_pipe.pipe[1], transaction, sizeof(struct transaction_t));
	pthread_mutex_unlock(&trtr_pipe.mutex);

	free(transaction);
}

static inline void trtrLOG(struct transaction_t * transaction){
	if(fwrite(&transaction, 1, sizeof(struct transaction_t), trtr_pipe.streams[transaction->market]) != sizeof(struct transaction_t))
		FATAL_ERROR("\nTransaction LOG fatal error: Fatal error of writing transactions log for %d market\n", transaction->market);
}

/*
static inline void trtrRebalance(struct transaction_t * transaction){
	struct {
		struct account_curr_key_t key;
		type_balance balance;
	} balances[4] = {
			{
					.key.account = transaction->side1,
					.key.curr = 1,
					.balance = transaction->maker_limit
			},
			{
					.key.account = transaction->side1,
					.key.curr = 2,
					.balance = transaction->maker_balance
			},
			{
					.key.account = transaction->side2,
					.key.curr = 1,
					.balance = transaction->taker_limit
			},
			{
					.key.account = transaction->side2,
					.key.curr = 2,
					.balance = transaction->taker_balance
			}
	};

	//redisBegin(trtr_pipe.redis);
	for(int i=0; i < 4; i++) {
		redisReply * reply = redisCommand(trtr_pipe.redis, "SET %lu_%d %llu",
				balances[i].key.account, balances[i].key.curr, balances[i].balance);
		//LOGD("SET: %s\n", reply->str);
		freeReplyObject(reply);
	}
	//redisEnd(trtr_pipe.redis);
}
*/

void * trtrProccess(void * ctx) {
	trtrInitStreams((struct trtr_context_t *)ctx);
	//trtrInitredis((struct trtr_context_t *)ctx);
	struct transaction_t transaction;
	while(1) {
		int size = read(trtr_pipe.pipe[0], &transaction, sizeof(transaction));
		if((size != sizeof(transaction)) || (transaction.market >= MARKETS_NUMBER))
			FATAL_ERROR("\nTransaction LOG fatal error: Error of transaction structure for %d market\n", transaction.market);
		trtrLOG(&transaction);
		trtr_pipe.transmited_to_steams++;
	}
}

void * trtrContextConstructor(yaml_document_t * document, yaml_node_t * node) {
	struct trtr_context_t * context = malloc(sizeof(struct trtr_context_t));

	yaml_node_t * fs_dir = yaml_node_get_child(document, node, "logdir");
	//yaml_node_t * redis_server = yaml_node_get_child(document, node, "redis_server");
	//yaml_node_t * redis_port = yaml_node_get_child(document, node, "redis_port");

	strcpy(&context->logdir[0], fs_dir ? yaml_node_get_value(fs_dir) : DEF_RTANSACTIONS_LOG_PATH);
	//strcpy(&context->redis_server[0], redis_server ? yaml_node_get_value(redis_server) : DEF_RTANSACTIONS_REDIS_SERVER);
	//context->redis_port = redis_port ? atoi(yaml_node_get_value(redis_port)) : DEF_RTANSACTIONS_REDIS_PORT;

	return context;
}

void trTransmiterInit(void) {
	if(pipe(trtr_pipe.pipe) == -1) {
		LOGE("Error of creating pipe for transactions\n");
		exit(1);
	}
	pthread_mutex_init(&trtr_pipe.mutex, NULL);
}
