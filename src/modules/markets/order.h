/*
 * order.h
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_MARKETS_ORDER_H_
#define MODULES_MARKETS_ORDER_H_

#include <stdint.h>
#include "../types.h"

#define BUY_MASK (unsigned char)0b10000000

enum order_type_t { 		//First bit is flag of buy (1) or sell (0) order
	otCancelOrder 		= 0b00000000,
	otDeposit			= 0b10000001,
	otWithdrow			= 0b00000010,
	otFixPriceBuy		= 0b10000011,
	otFixPriceSell		= 0b00000100,
	otMarketPriceBuy	= 0b10000101,
	otMarketPriceSell	= 0b00000110
};

struct __attribute__((__packed__)) order_t {
	type_id 		id;
	type_type  		type;
	type_market 	market;
	type_account 	account;
	type_stock  	stock;
	type_price 		price;
	type_limit 		limit;
	type_ext_type  extended;
};

#endif /* MODULES_MARKETS_ORDER_H_ */
