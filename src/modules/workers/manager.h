/*
 * executer.h
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_WORKERS_MANAGER_H_
#define MODULES_WORKERS_MANAGER_H_

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <pthread.h>
#include <yaml.h>
#include "../../consts.h"
#include "../gateways/rabbitmq.h"

struct process_context_t {
	_Bool disable;
	struct {
		struct rmq_context_t orders;
		struct rmq_context_t transactions;
	} rmq;
};

typedef void * (*context_constructor_t)(yaml_document_t * document, yaml_node_t * node);

struct class_process {
	char class[255];
	context_constructor_t context_constructor;	//Context constructor
	void * handle;								//Main function of process
	_Bool require;								//Process is require
	_Bool single;								//Available only one process
	int started;								//Counter of started processes
};

pthread_t executers[MAX_EXCUTERS_NUMBER];

void startExecuters(void);

#endif /* MODULES_WORKERS_MANAGER_H_ */
