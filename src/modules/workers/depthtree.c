/*
 * depthtree.c
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */
#include "../workers/depthtree.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "../markets/transaction.h"
#include "../markets/order.h"
#include "../../consts.h"
#include "../utils.h"
#include "../types.h"
#include "../workers/manager.h"

struct market_node_t markets[MARKETS_NUMBER] = {0};

struct transaction_counter_t transaction_counter =
{
		.mutex_transaction 	= PTHREAD_MUTEX_INITIALIZER,
		.counter			= 0
};

type_id bindTransactionID(void) {
	return ++transaction_counter.counter;
}

static inline struct depth_row_t * newDepthRow(){
	struct depth_row_t * result = malloc(sizeof(struct depth_row_t));
	memset(result, 0, sizeof(struct depth_row_t));
	return result;
}

static inline void freeDepthRow(struct depth_row_t * row){
	for(struct order_unit_t * order = row->first_order; order != NULL;) {
		void * pointer = order;
		order = order->next;
		free(pointer);
	}
	free(row);
}

static inline struct order_unit_t * newOrderUnit(struct order_t * order){
	struct order_unit_t * result = malloc(sizeof(struct order_unit_t));
	result->order = *order;
	result->next = NULL;
	return result;
}

void collectUnitOrder(struct order_unit_t * unit_order){
	free(unit_order);
}

void collectDepthRow(struct depth_row_t * row){
	free(row);
}

static inline void lockDepth(struct depth_of_stock_t * depth){
	pthread_mutex_lock(&depth->mutex);
}

static inline void unlockDepth(struct depth_of_stock_t * depth){
	pthread_mutex_unlock(&depth->mutex);
}

void makeOrder(struct order_t * order, struct depth_of_stock_t * depth) {
	int isSuccess = 0;
	_Bool isBuy = order->type & BUY_MASK;
	struct depth_row_t ** depth_header = isBuy ? &depth->buy : &depth->sell;
	type_volume * depth_volume = isBuy ? &depth->volume_buy : &depth->volume_sell;
	if(*depth_header == NULL) {	//If buy part of depth is empty
		struct depth_row_t * new_row = newDepthRow();
		new_row->limit = order->limit;
		new_row->price = order->price;
		new_row->first_order = newOrderUnit(order);
		new_row->last_order = new_row->first_order;
		*depth_header = new_row;
		*depth_volume += order->limit;
	} else { //Append order to depth
		struct depth_row_t * curr_row = *depth_header;
		struct depth_row_t * prev_row = NULL;
		while(curr_row != NULL && isSuccess == 0) {
			if(curr_row->price == order->price) {
				struct order_unit_t * unit = newOrderUnit(order);
				curr_row->limit += order->limit;
				curr_row->last_order->next = unit;
				curr_row->last_order = unit;
				*depth_volume += order->limit;
				isSuccess = 1;
				curr_row = NULL;
				break;
			} else if(
					(isBuy && (order->price > curr_row->price)) ||
					(!isBuy && (order->price < curr_row->price)) ||
					(curr_row->next == NULL)
					) {
				struct depth_row_t * new_row = newDepthRow();
				new_row->limit = order->limit;
				new_row->price = order->price;
				new_row->first_order = newOrderUnit(order);
				new_row->last_order = new_row->first_order;
				if(!(
						(isBuy && (order->price > curr_row->price)) ||
						(!isBuy && (order->price < curr_row->price))
					)) {
					curr_row->next = new_row;
				} else if(curr_row == *depth_header) {
					new_row->next = curr_row;
					*depth_header = new_row;
				} else {
					new_row->next = curr_row;
					if(prev_row == NULL)
						*depth_header = new_row;
					else
						prev_row->next = new_row;
				}
				*depth_volume += order->limit;
				isSuccess = 1;
				curr_row = NULL;
			}
			prev_row = curr_row;
			if(curr_row != NULL)
				curr_row = curr_row->next;
		}
	}
}

type_volume matchOrder(struct depth_row_t * row, struct order_t * order) {
	struct order_unit_t * unit = row->first_order;
	type_volume result = 0;
	while((unit != NULL) && (order->limit > 0)) {
		struct transaction_t * transaction = newTransaction();
		transaction->market = order->market;
		transaction->moment = time(NULL);
		transaction->stock = order->stock;
		transaction->price = row->price;
		transaction->side1 = unit->order.account;
		transaction->side2 = order->account;

		if(unit->order.limit >= order->limit) {
			transaction->volume  = order->limit;
			unit->order.limit 	-= order->limit;
			row->limit 			-= order->limit;
			result 				+= order->limit;
			order->limit = 0;
		} else {
			transaction->volume  = unit->order.limit;
			order->limit 		-= unit->order.limit;
			row->limit 			-= unit->order.limit;
			result		 		+= unit->order.limit;
			unit->order.limit = 0;
		}

		transaction->id = bindTransactionID();
		transmitTransaction(transaction);

		if(unit->order.limit == 0) {
			struct order_unit_t * next = unit->next;
			collectUnitOrder(unit);
			unit = next;
		}
	}
	row->first_order = unit;
	return result;
}

void doMatchOrMakeBuySellOrder(struct order_t * order, struct depth_of_stock_t * depth) {
	_Bool isBuy = order->type & BUY_MASK;
	struct depth_row_t * header = isBuy ? depth->sell : depth->buy;
	struct depth_row_t * curr_row = header;
	type_volume * depth_volume = isBuy ? &depth->volume_sell : &depth->volume_buy;
	while( (curr_row != NULL) && (order->limit > 0) && (
			(isBuy && (curr_row->price <= order->price)) ||
			(!isBuy && (curr_row->price >= order->price))
		)) {
		*depth_volume -= matchOrder(curr_row, order);
		if(curr_row->first_order == NULL) {
			struct depth_row_t * next = curr_row->next;
			collectDepthRow(curr_row);
			curr_row = next;
		}
	}

	if(isBuy)
		depth->sell = curr_row;
	else
		depth->buy = curr_row;

	if(order->limit > 0)
		makeOrder(order, depth);
}

int processOrder(struct process_context_t * context, struct order_t * order) {
	if(order->market >= MARKETS_NUMBER)
		return MAKE_ERROR_UNKNOWN_MARKET;
	struct market_node_t * market = &markets[order->market];
	struct depth_of_stock_t * depth = &market->stocks[order->stock];

	lockDepth(depth);
	switch (order->type){
		case otFixPriceBuy:
		case otMarketPriceBuy:
			doMatchOrMakeBuySellOrder(order, depth);
			break;
		case otFixPriceSell:
		case otMarketPriceSell:
			doMatchOrMakeBuySellOrder(order, depth);
			break;
		default:
			LOGE("Unknown order [%d] type [%d]", order->id, order->type);
	}
	unlockDepth(depth);
	return 0;
}


