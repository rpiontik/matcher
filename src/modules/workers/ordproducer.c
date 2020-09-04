/*
 * ordproducer.c
 *
 *  Created on: 13 авг. 2018 г.
 *      Author: rpiontik
 */

#include "../workers/ordproducer.h"

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "../markets/order.h"
#include "../utils.h"
#include "manager.h"
#include "accounts.h"

static int order_number = 0;

void generateOrder(struct order_t * order) {
	order->id = order_number++; //time(NULL)
	order->account = rand() % 100;

	//RAND_MAX
	order->market = 0; //syscall(__NR_gettid) % 16;
	order->stock = 0; //rand() % 128;

	switch(rand() % 4){
		case 0:
			order->type = otFixPriceBuy;
			break;
		case 1:
			order->type = otFixPriceSell;
			break;
		case 2:
			order->type = otMarketPriceBuy;
			break;
		case 3:
			order->type = otMarketPriceSell;
			break;
	}
	order->limit = rand() % 1 + 1;
	order->price = rand() % 1 + 1;

	accountsLock();
	struct account_t * account = findAccount(order->account);
	if(account == NULL) {
		account = appendAccount(order->account);
		//memset(&account->balances, 0xFF, sizeof(account->balances));
	}
	accountsUnlock();
}

void die_on_error(int x, char const *context) {
  if (x < 0) {
    fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x));
    exit(1);
  }
}

void * orderProducerProccess(void * ctx){
	struct rmq_order_t order;
	struct rmq_context_t * context = (struct rmq_context_t *)ctx;
	unsigned long sent = 0;
	uint64_t lastime = now_microseconds();
	while(1) {
		if(!context->isConnected) {
			if(!connectToRabbitMQ(context))
				sleep(1);
			continue;
		}

		generateOrder(&order.order);
		order.crc32 = crc32((unsigned char*)&order.order, sizeof(struct order_t), 0);

		amqp_bytes_t packed;
		packed.len = sizeof(struct rmq_order_t);
		packed.bytes = &order;

		if(amqp_basic_publish(
				context->conn,
				1,
				amqp_cstring_bytes(context->exchange),
				amqp_cstring_bytes(context->routing_key),
				0,
				0,
				NULL,
				packed)) {
			disconnectRabbitMQ(context);
			LOGI("\nOrder producer is reconnecting...\n");
			continue;
		}

		sent++;
		if(now_microseconds() - lastime > 1000000) {
			LOGI("\nRabbitMQ sent performance %lu per sec\n", sent);
			sent = 0;
			lastime = now_microseconds();
		}
	}
	return NULL;
}
