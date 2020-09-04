/*
 * redis.h
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_GATEWAYS_REDIS_H_
#define MODULES_GATEWAYS_REDIS_H_

#include "../../libs/hiredis/hiredis.h"
#include "../types.h"


struct account_curr_key_t {
	type_account account;
	type_curr curr;
};


_Bool redisBegin(redisContext * redis);
_Bool redisEnd(redisContext * redis);

#endif /* MODULES_GATEWAYS_REDIS_H_ */
