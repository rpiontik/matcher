/*
 * redis.c
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: rpiontik
 */

#include "redis.h"

#include "../../libs/hiredis/hiredis.h"


inline _Bool redisBegin(redisContext * redis) {
	redisReply *reply = redisCommand(redis, "MULTI");
	freeReplyObject(reply);
	return 1;
}

inline _Bool redisEnd(redisContext * redis) {
	redisReply *reply = redisCommand(redis, "EXEC");
	freeReplyObject(reply);
	return 1;
}

