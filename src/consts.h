/*
 * consts.h
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef CONSTS_H_
#define CONSTS_H_

#define DEBUG_MODE

#define MAX_EXCUTERS_NUMBER 				32	//Maximum number workers
#define MARKETS_NUMBER 						16 	//Actual markets
#define MAX_CURR_NUMBER						32 	//Maximum number of balances per account

#define DEF_MATCHER_PIPE_COUNT	 			256

#define DEF_RTANSACTIONS_LOG_PATH			""
#define DEF_RTANSACTIONS_REDIS_SERVER		"127.0.0.1"
#define DEF_RTANSACTIONS_REDIS_PORT			6379

#define DEF_ACCOUNTS_DUMP_INTERVAL 			1000000
#define DEF_ACCOUNTS_DUMP_PATH	 			""
#define DEF_ACCOUNTS_MEM_BLOCK_RECS			500

#define DEF_RMQ_PREFETCH_COUNT	 			1000
#define DEF_RMQ_FRAME_MAX		 			0xFFFFFFF
#define DEF_RMQ_CHANNELS_MAX				0xFFFF
#define DEF_RMQ_SASL_METHOD					AMQP_SASL_METHOD_PLAIN
#define DEF_RMQ_PROC_TYPE_MATCHER			"matcher"
#define DEF_RMQ_PROC_TYPE_ORDER_PRODUCER	"order-producer"

#define DEF_RMQ_ORDER_EXCHANGE  			"orders"
#define DEF_RMQ_ORDER_ROUTINGKEY			"market"
#define DEF_RMQ_ORDER_VHOST		 			"matcher"
#define DEF_RMQ_ORDER_QUEUENAME	 			"incoming"
#define DEF_RMQ_ORDER_SERVER	 			"192.168.1.197"
#define DEF_RMQ_ORDER_PORT		 			5672
#define DEF_RMQ_ORDER_LOGIN		 			"admin"
#define DEF_RMQ_ORDER_PASS		 			"admin"


#endif /* CONSTS_H_ */
