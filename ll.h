#ifndef ABOUT_LLS_H
#define ABOUT_LLS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct ab_info_t {
	void *data;
	long address;
	int data_size;
} ab_info_t;

typedef struct ll_node_t {
	void *data;
	struct ll_node_t *next;
	//int data_size;
	//long address;
} ll_node_t;

typedef struct ll_list_t {
	ll_node_t *head;
	//unsigned int data_size;
	unsigned int size;
} ll_list_t;

ll_list_t *ll_create(void);

void ll_add_nth_node(ll_list_t *list, unsigned int n, int data_size,
					 long address);

ll_node_t *ll_remove_nth_node(ll_list_t *list, unsigned int n);

unsigned int ll_get_size(ll_list_t *list);

void ll_free(ll_list_t **pp_list);

void ll_print_int(ll_list_t *list);

void ll_print_string(ll_list_t *list);

#endif
