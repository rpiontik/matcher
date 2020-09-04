/*
 * utils.h
 *
 *  Created on: 6 авг. 2018 г.
 *      Author: rpiontik
 */

#ifndef MODULES_UTILS_H_
#define MODULES_UTILS_H_

#include <stdio.h>
#include <yaml.h>
#include <stdint.h>

#define LOGI(...)	fprintf(stdout, __VA_ARGS__)
#define LOGD(...)	fprintf(stdout, __VA_ARGS__)
#define LOGE(...)	fprintf(stderr, __VA_ARGS__)
#define FATAL_ERROR(...)	{fprintf(stderr, __VA_ARGS__); exit(1);}	/* TERMINATE PROCESS */
#define CRITICAL_ERROR(...)	{fprintf(stderr, __VA_ARGS__);}	/* REPORT REGARDING ERROR */

yaml_node_t* yaml_node_get_child(yaml_document_t* document, yaml_node_t* parent_node, char* key);
yaml_node_t* yaml_node_get_element(yaml_document_t* document, yaml_node_t* sequence_node, int index);
char * yaml_node_get_value(yaml_node_t* node);

uint32_t crc32(const unsigned char *buf, int len, unsigned int init);

uint64_t now_microseconds(void);

void fatalError(char * error);


struct btree_node_t {
	struct btree_node_t * left;
	struct btree_node_t * right;
	void * location;
};

struct btree_node_t * createBTreeNode(void);
void appendBTreeKey(struct btree_node_t * tree, uint32_t key, void * location);
void * findBTreeLocation(struct btree_node_t * tree, uint32_t key);

#endif /* MODULES_UTILS_H_ */
