/*
 * orders.c
 *
 *  Created on: 14 авг. 2018 г.
 *      Author: rpiontik
 */

#include "ordreceiver.h"

#include <unistd.h>
#include "../markets/order.h"
#include "../utils.h"
#include "../workers/depthtree.h"
#include "matchcore.h"

void * orderReceiverProccess(void * cxt) {
	int current_pipe = 0;
	struct rmq_context_t * context = (struct rmq_context_t *)cxt;
	struct order_t orders[context->prefetch_count];
	while(1) {
		if(!context->isConnected) {
			if(!connectToRabbitMQ(context))
				sleep(1);
			continue;
		}

		int received = reciveOrdersFromRabbitMQ(context, orders);

		while(received > 0) {
			if(pthread_mutex_trylock(&mather_pipes[current_pipe].mutex) == 0){
				mather_pipes[current_pipe].orders = malloc(received * sizeof(struct order_t));
				mather_pipes[current_pipe].volume = received;
				memcpy(mather_pipes[current_pipe].orders, &orders, received * sizeof(struct order_t));
				pthread_mutex_unlock(&mather_pipes[current_pipe].mutex);
				received = 0;
			}
			current_pipe++;
			if(current_pipe >= DEF_MATCHER_PIPE_COUNT)
				current_pipe = 0;
		}
	}
	return NULL;
}
