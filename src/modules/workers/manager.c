/*
 * executer.c
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 *      https://habr.com/post/122108/
 */
#include "manager.h"

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <yaml.h>
#include "../../consts.h"
#include "../utils.h"
#include "matchcore.h"
#include "ordreceiver.h"
#include "ordproducer.h"
#include "trtransmit.h"
#include "accounts.h"

#define NUMBER_PROC_CLASSES 5

struct class_process classes_proc[NUMBER_PROC_CLASSES]	= {
		{
				.class 	= "matcher",
				.handle = &matcherProccess,
				.context_constructor = NULL,
				.require = 1,
				.single = 1,
				.started  = 0
		},
		{
				.class 	= "accounts-dumper",
				.handle = &accountDumpProccess,
				.context_constructor = &accountContextConstructor,
				.require = 0,
				.single = 1,
				.started  = 0
		},
		{
				.class 	= "orders",
				.handle = &orderReceiverProccess,
				.context_constructor = &rmqContextConstructor,
				.require = 1,
				.single = 0,
				.started  = 0
		},
		{
				.class	= "order-producer",
				.handle = &orderProducerProccess,
				.context_constructor = &rmqContextConstructor,
				.require = 0,
				.single = 0,
				.started  = 0
		},
		{
				.class	= "tr-transmitter",
				.handle = &trtrProccess,
				.context_constructor = &trtrContextConstructor,
				.require = 1,
				.single = 1,
				.started  = 0
		}
};

void startExecuters(void) {
	yaml_parser_t parser;
	yaml_document_t document;

	FILE *config = fopen("config.yml", "rb");
	if(!config) {
		LOGE("\nCould not open config.yml\n");
		exit(1);
	}

	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, config);

	if(!yaml_parser_load(&parser, &document)) {
		LOGE("\nError loading config.yml file\n");
		exit(1);
	}

	yaml_node_t * root = yaml_document_get_root_node(&document);

	yaml_node_t* processes_list = yaml_node_get_child(&document, root, "processes");
	int proc_counter = 0;
	int launched = 0;
	if(processes_list) {
		yaml_node_t * process;
		for (;
				proc_counter < MAX_EXCUTERS_NUMBER &&
				(process = yaml_node_get_element(&document, processes_list, proc_counter)) != NULL;
				proc_counter++
			) {

			yaml_node_t * yml_class = yaml_node_get_child(&document, process, "class");
			yaml_node_t * yml_name = yaml_node_get_child(&document, process, "name");
			yaml_node_t * yml_disable = yaml_node_get_child(&document, process, "disable");
			char str_class[255] = {0};
			char str_name[255] = {0};
			strcpy(&str_class[0], yml_class ? yaml_node_get_value(yml_class) : DEF_RMQ_PROC_TYPE_MATCHER);
			strcpy(&str_name[0], yml_name ? yaml_node_get_value(yml_name) : "noname");

			if(yml_disable && atoi(yaml_node_get_value(yml_disable)) > 0) {
				LOGI("Disabled process is skipped %s:%s\n", str_class, str_name);
				continue;
			}

			_Bool isLaunched = 0;
			for(int i = 0; i < NUMBER_PROC_CLASSES; i++) {
				if(strcmp(classes_proc[i].class, str_class) == 0) {
					if(classes_proc[i].single && classes_proc[i].started)
						FATAL_ERROR("\nYou can not start more than one process for %s class\n", str_class);
					void * context = classes_proc[i].context_constructor ? classes_proc[i].context_constructor(&document, process) : NULL;
					if(pthread_create(&(executers[proc_counter]), NULL, classes_proc[i].handle, context)) {
						LOGE("Could not start process %s:%s\n", str_class, str_name);
						exit(1);
					} else {
						LOGI("Started process %s:%s\n", str_class, str_name);
					}
					classes_proc[i].started++;
					isLaunched  = 1;
					sleep(1);
					break;
				}
			}
			if(isLaunched)
				launched++;
			else {
				LOGE("Unknown class %s:%s\n", str_class, str_name);
				exit(1);
			}
		}
	}

	for(int i = 0; i < NUMBER_PROC_CLASSES; i++) {
		if(classes_proc[i].require && !classes_proc[i].started)
			FATAL_ERROR("\nClass %s must have minimum one instance\n", classes_proc[i].class);
	}

	if(!launched) {
		LOGE("No any process launched :(\n");
		exit(1);
	}

	LOGI("Started %d processes (max %d)\n", launched, MAX_EXCUTERS_NUMBER);

	yaml_document_delete(&document);
	yaml_parser_delete(&parser);
	fclose(config);
}
