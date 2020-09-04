/*
 * rabbitmq.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_GATEWAYS_RABBITMQ_H_
#define MODULES_GATEWAYS_RABBITMQ_H_

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <yaml.h>
#include "../markets/order.h"

struct rmq_context_t {
	_Bool isConnected;
	char vhost[255];
	char hostname[255];
	char queuename[255];
	char exchange[255];
	char routing_key[255];
	int channel_max;
	int frame_max;
	int heartbeat;
	amqp_sasl_method_enum sasl_method;
	char login[255];
	char pass[255];
	char bindingkey[255];
	int port;
	int status;
	int prefetch_count;
	amqp_socket_t *socket;
	amqp_connection_state_t conn;
};

struct __attribute__((__packed__)) rmq_order_t {
	uint32_t crc32;
	struct order_t order;
};

_Bool connectToRabbitMQ(struct rmq_context_t * context);
void disconnectRabbitMQ(struct rmq_context_t * context);
int reciveOrdersFromRabbitMQ(struct rmq_context_t * context, struct order_t * orders);
void * rmqContextConstructor(yaml_document_t * document, yaml_node_t * node);

#endif /* MODULES_GATEWAYS_RABBITMQ_H_ */
