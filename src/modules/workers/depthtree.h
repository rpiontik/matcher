/*
 * depthtree.h
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_WORKERS_DEPTHTREE_H_
#define MODULES_WORKERS_DEPTHTREE_H_

#include <pthread.h>
#include <stdint.h>
#include "../markets/order.h"
#include "../markets/transaction.h"
#include "../../consts.h"
#include "../types.h"
#include "manager.h"

struct order_unit_t {
	struct order_t order;					//Order body
	struct order_unit_t * next;			//Pointer to next order unit
};

struct depth_row_t {
	uint64_t price;						//Price row of depth
	uint64_t limit;						//Total limit of orders in the row
	struct order_unit_t * first_order;	//Pointer to first order in the depth
	struct order_unit_t * last_order;		//Pointer to last order in the depth
	struct depth_row_t * 	next;			//Pointer to next row in the depth
};

struct depth_of_stock_t {
	pthread_mutex_t 	mutex;			//
	type_volume			volume_buy;		//Buy volume of depth
	type_volume			volume_sell;	//Sell volume of depth
	struct depth_row_t 	* buy;			//Pointer to first buy row
	struct depth_row_t 	* sell;			//Pointer to first sell row
};

struct market_node_t {
	struct depth_of_stock_t stocks[256];	//Stocks of market
};

extern struct market_node_t markets[MARKETS_NUMBER];	//Markets of exchange

extern struct transaction_counter_t {
	pthread_mutex_t 	mutex_transaction;
	type_id				counter;							//Transaction ID counter
} transaction_counter;

#define MAKE_ERROR_UNKNOWN_MARKET	0x01

//Reserve transaction id
type_id bindTransactionID(void);

//Push the order at depth or match orders to transaction
int processOrder(struct process_context_t * context, struct order_t * order);

#endif /* MODULES_WORKERS_DEPTHTREE_H_ */
