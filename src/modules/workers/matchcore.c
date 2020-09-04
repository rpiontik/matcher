/*
 * matcher.c
 *
 *  Created on: 13 авг. 2018 г.
 *      Author: rpiontik
 */

#define _GNU_SOURCE

#include "matchcore.h"

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "../markets/order.h"
#include "../workers/depthtree.h"
#include "../utils.h"
#include "../../consts.h"

unsigned long incoming_packet_counter  = 0;
struct mather_pipe_t mather_pipes[DEF_MATCHER_PIPE_COUNT] = {0};

extern pthread_mutex_t match_mutex;
extern pthread_mutex_t request_mutex;

pthread_mutex_t match_mutex = {0};
pthread_mutex_t request_mutex = {0};

void * matcherProccess(void * context){
	int current_pipe = 0;
	while(1) {
		if(mather_pipes[current_pipe].volume >0 && (pthread_mutex_trylock(&mather_pipes[current_pipe].mutex) == 0)){
			incoming_packet_counter += mather_pipes[current_pipe].volume;
			for(unsigned long i = 0; i < mather_pipes[current_pipe].volume; i++)
				processOrder(context, &mather_pipes[current_pipe].orders[i]);
			mather_pipes[current_pipe].volume = 0;
			free(mather_pipes[current_pipe].orders);
			pthread_mutex_unlock(&mather_pipes[current_pipe].mutex);
		}
		current_pipe++;
		if(current_pipe >= DEF_MATCHER_PIPE_COUNT)
			current_pipe = 0;
	}
	return NULL;
}

void matcherProcsInit(void) {
	for(int i = 0; i < DEF_MATCHER_PIPE_COUNT; i++) {
		pthread_mutex_init(&mather_pipes[i].mutex, NULL);
	}
}


