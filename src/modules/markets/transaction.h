/*
 * transaction.h
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_MARKETS_TRANSACTION_H_
#define MODULES_MARKETS_TRANSACTION_H_


#include <stdint.h>
#include "../types.h"

enum transaction_type_t { 		//First bit is flag of buy (1) or sell (0) order
	trBuy				= 0b10000011,
	trSell				= 0b00000100
};

struct __attribute__((__packed__)) transaction_t {
	type_id 		id;
	type_timestamp 	moment;
	type_type 		type;
	type_market 	market;
	type_stock  	stock;
	type_account 	side1;
	type_account 	side2;
	type_price 		price;
	type_volume 	volume;
	type_balance 	maker_balance;
	type_balance 	maker_limit;
	type_balance 	taker_balance;
	type_balance 	taker_limit;
};

struct transaction_t * newTransaction();


#endif /* MODULES_MARKETS_TRANSACTION_H_ */
