/*
 * matcher.h
 *
 *  Created on: 13 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_WORKERS_MATCHCORE_H_
#define MODULES_WORKERS_MATCHCORE_H_

#include <stdint.h>
#include <pthread.h>
#include "../../consts.h"

struct mather_pipe_t {
	uint32_t volume;
	struct order_t * orders;
	pthread_mutex_t mutex;
};

extern struct mather_pipe_t mather_pipes[DEF_MATCHER_PIPE_COUNT];

extern uint64_t incoming_packet_counter;


void * matcherProccess(void * context);
void matcherProcsInit(void);

#endif /* MODULES_WORKERS_MATCHCORE_H_ */
