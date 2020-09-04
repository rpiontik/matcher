/*
 * transaction.c
 *
 *  Created on: 8 авг. 2018 г.
 *      Author: rpiontik
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transaction.h"
#include "../utils.h"
#include "../workers/depthtree.h"

struct transaction_t * newTransaction() {
	struct transaction_t * result = malloc(sizeof(struct transaction_t));
	memset(result, 0, sizeof(struct transaction_t));
	return result;
}


