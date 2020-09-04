/*
 * rabbitmq.c
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: rpiontik
 */

#include "rabbitmq.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "../utils.h"
#include "../../consts.h"

_Bool isRMQError(amqp_rpc_reply_t reply, char * context) {
	pid_t pid = syscall(__NR_gettid);
	switch (reply.reply_type) {
		case AMQP_RESPONSE_NORMAL: {
			LOGI("(%d) %s: success\n", pid, context);
			return 0;
		}
		case AMQP_RESPONSE_NONE: {
			LOGE("(%d) %s, error of connecting to RabbitMQ: missing RPC reply type!\n", pid, context);
			return 1;
		}
		case AMQP_RESPONSE_LIBRARY_EXCEPTION: {
			LOGE("(%d) %s: RabbitMQ library exception [%s]\n", pid, context, amqp_error_string2(reply.library_error));
			return 1;
		}
		case AMQP_RESPONSE_SERVER_EXCEPTION: {
			switch (reply.reply.id) {
				case AMQP_CONNECTION_CLOSE_METHOD: {
					amqp_connection_close_t *m = (amqp_connection_close_t *)reply.reply.decoded;
					LOGE("(%d) %s: RabbitMQ server connection error %uh, message: %.*s\n",
							pid,
							context,
							m->reply_code, (int)m->reply_text.len,
							(char *)m->reply_text.bytes);
					break;
			}
			case AMQP_CHANNEL_CLOSE_METHOD: {
				amqp_channel_close_t *m = (amqp_channel_close_t *)reply.reply.decoded;
				LOGE("(%d) %s: RabbitMQ server channel error %uh, message: %.*s\n",
						pid,
						context,
						m->reply_code, (int)m->reply_text.len,
						(char *)m->reply_text.bytes
						);
				break;
			}
			default:
				LOGE("(%d) %s: unknown RabbitMQ server error, method id 0x%08X\n",
						pid,
						context,
						reply.reply.id
						);
				break;
		  }
		  return 1;
		}
		default:
			LOGE("(%d) %s: Unknown RabbitMQ error\n", pid, context);
			return 1;
	}
}

_Bool connectToRabbitMQ(struct rmq_context_t * context){
	context->isConnected = 0;

	if(!context->conn) {
		context->conn = amqp_new_connection();
		context->socket = amqp_tcp_socket_new(context->conn);
		if (!context->socket) {
			LOGE("RabbitMQ: Could not create TCP socket\n");
			amqp_destroy_connection(context->conn);
			context->conn = NULL;
			exit(1);
		}
	}

	LOGI("(%d) Trying to connect to RabbitMQ with params vhost=[%s], channel_max=[%d], frame_max=[%d], heartbeat=[%d], sasl_method=[%d], login=[%s]\n",
			syscall(__NR_gettid),
			context->vhost,
			context->channel_max,
			context->frame_max,
			context->heartbeat,
			context->sasl_method,
			context->login
			);

	if(amqp_socket_open(context->socket, context->hostname,context->port)) {
		LOGE("(%d) Error of connection to %s:%d\n", syscall(__NR_gettid), context->hostname, context->port);
		amqp_destroy_connection(context->conn);
		context->conn = NULL;
		return 0;
	}

	if(isRMQError(amqp_login(
				context->conn,
				context->vhost,
				context->channel_max,
				context->frame_max,
				context->heartbeat,
				context->sasl_method,
				context->login,
				context->pass
			), "Connecting")) {
		amqp_destroy_connection(context->conn);
		context->conn = NULL;
		return 0;
	}

	amqp_channel_open(context->conn, 1);
	if(isRMQError(amqp_get_rpc_reply(context->conn), "Channel.open")) {
		amqp_destroy_connection(context->conn);
		context->conn = NULL;
		return 0;
	}

	amqp_basic_qos(context->conn, 1, 0, context->prefetch_count, 0);
	if(amqp_get_rpc_reply(context->conn).reply_type != AMQP_RESPONSE_NORMAL) {
		amqp_destroy_connection(context->conn);
		context->conn = NULL;
		return 0;
	}

	context->isConnected = 1;

	return 1;
}

void disconnectRabbitMQ(struct rmq_context_t * context){
	context->isConnected = 0;
	if(context->conn) {
		amqp_destroy_connection(context->conn);
		context->conn = NULL;
	}
}

int reciveOrdersFromRabbitMQ(struct rmq_context_t * context, struct order_t * orders) {

	int result = 0;

	amqp_basic_consume(context->conn, 1, amqp_cstring_bytes(&context->queuename[0]),
			amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
	if(amqp_get_rpc_reply(context->conn).reply_type != AMQP_RESPONSE_NORMAL) {
		amqp_destroy_connection(context->conn);
		context->conn = NULL;
		return 0;
	}

	for(int i = 0; i < context->prefetch_count; i++) {
		amqp_frame_t frame;
		if(amqp_simple_wait_frame(context->conn, &frame) != AMQP_STATUS_OK)
			return 0;

		if((frame.frame_type == AMQP_FRAME_METHOD) &&
				(frame.payload.method.id == AMQP_BASIC_DELIVER_METHOD)) {

			amqp_message_t message;
			amqp_rpc_reply_t  rc_read = amqp_read_message (context->conn, frame.channel, &message, 0);

			if (rc_read.reply_type != AMQP_RESPONSE_NORMAL)
				break;

			if(message.body.len != sizeof(struct rmq_order_t)) {
				LOGE("Error of order content require %lu received %lu",
						sizeof(struct rmq_order_t),
						message.body.len
				);

				break;
			}

			struct rmq_order_t * rmq_order = (struct rmq_order_t *)message.body.bytes;
			memcpy(&orders[result], &rmq_order->order, sizeof(struct order_t));

			result++;

			amqp_destroy_message (&message);
		}
	}
	amqp_maybe_release_buffers(context->conn);

	return result;

}

void * rmqContextConstructor(yaml_document_t * document, yaml_node_t * node) {
	struct rmq_context_t * context = malloc(sizeof(struct rmq_context_t));
	memset(context, 0, sizeof(struct rmq_context_t));

	yaml_node_t * vhost			= NULL;
	yaml_node_t * hostname		= NULL;
	yaml_node_t * queuename		= NULL;
	yaml_node_t * port			= NULL;
	yaml_node_t * login			= NULL;
	yaml_node_t * pass			= NULL;
	yaml_node_t * exchange		= NULL;
	yaml_node_t * routing_key	= NULL;
	yaml_node_t * prefetch_count= NULL;

	if(node) {
		vhost			= yaml_node_get_child(document, node, "vhost");
		hostname		= yaml_node_get_child(document, node, "server");
		queuename		= yaml_node_get_child(document, node, "queue");
		port			= yaml_node_get_child(document, node, "port");
		login			= yaml_node_get_child(document, node, "login");
		pass			= yaml_node_get_child(document, node, "pass");
		exchange		= yaml_node_get_child(document, node, "exchange");
		routing_key		= yaml_node_get_child(document, node, "routing_key");
		prefetch_count 	= yaml_node_get_child(document, node, "prefetch_count");
	}

	context->frame_max = DEF_RMQ_FRAME_MAX;
	context->channel_max = DEF_RMQ_CHANNELS_MAX;
	context->sasl_method = DEF_RMQ_SASL_METHOD;

	strcpy(&context->vhost[0], vhost ? yaml_node_get_value(vhost) : DEF_RMQ_ORDER_VHOST);
	strcpy(&context->queuename[0], queuename ? yaml_node_get_value(queuename) : DEF_RMQ_ORDER_QUEUENAME);
	strcpy(&context->hostname[0], hostname ? yaml_node_get_value(hostname) : DEF_RMQ_ORDER_SERVER);
	strcpy(&context->login[0], login ? yaml_node_get_value(login) : DEF_RMQ_ORDER_LOGIN);
	strcpy(&context->pass[0], pass ? yaml_node_get_value(pass) : DEF_RMQ_ORDER_PASS);
	strcpy(&context->exchange[0], exchange ? yaml_node_get_value(exchange) : DEF_RMQ_ORDER_EXCHANGE);
	strcpy(&context->routing_key[0], routing_key ? yaml_node_get_value(routing_key) : DEF_RMQ_ORDER_ROUTINGKEY);
	context->port = port ? atoi(yaml_node_get_value(port)) : DEF_RMQ_ORDER_PORT;
	context->prefetch_count = prefetch_count ? atoi(yaml_node_get_value(prefetch_count)) : DEF_RMQ_PREFETCH_COUNT;

	return context;
}

