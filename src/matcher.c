/*
 ============================================================================
 Name        : matcher.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <assert.h>

#include "modules/utils.h"
#include "modules/workers/depthtree.h"
#include "modules/workers/manager.h"
#include "modules/workers/matchcore.h"
#include "modules/workers/trtransmit.h"

void init(void){
	trTransmiterInit();
	matcherProcsInit();
}

int main(int argc, char const *const *argv) {
	init();
	startExecuters();
	static uint64_t lastime = 0;
	static uint64_t lastid_transaction = 0;
	static uint64_t last_packet = 0;
	static uint64_t last_log_trans = 0;
	while(1) {
		if(lastime == 0) {
			lastime = now_microseconds();
			last_packet = incoming_packet_counter;
			lastid_transaction = transaction_counter.counter;
			last_log_trans = trtr_pipe.transmited_to_steams;
		} else if(now_microseconds() - lastime > 1000000) {
			LOGD("\nTransactions performance %lu/sec, packets %lu/sec, loged transactions %lu/sec",
					transaction_counter.counter - lastid_transaction,
					incoming_packet_counter - last_packet,
					trtr_pipe.transmited_to_steams - last_log_trans
					);
			lastid_transaction = transaction_counter.counter;
			last_packet = incoming_packet_counter;
			last_log_trans = trtr_pipe.transmited_to_steams;
			lastime = now_microseconds();
		}
	}
}
