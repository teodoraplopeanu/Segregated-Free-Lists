#ifndef ABOUT_DLLS_H
#define ABOUT_DLLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

typedef struct sfl_info_t {
	void *data;
	long address;
} sfl_info_t;

typedef struct dll_node_t dll_node_t;
struct dll_node_t {
	void *data;
	dll_node_t *prev, *next;
	//long address;
};

typedef struct dll_list_t {
	dll_node_t *head;
	unsigned int data_size;
	unsigned int size;
	long start_address;
} dll_list_t;

dll_list_t *dll_create(unsigned int data_size);

dll_node_t *dll_get_nth_node(dll_list_t *list, unsigned int n);

void dll_add_nth_node(dll_list_t *list, unsigned int n, long address);

dll_node_t *dll_remove_nth_node(dll_list_t *list, unsigned int n);

unsigned int dll_get_size(dll_list_t *list);

void dll_free(dll_list_t **pp_list);

void dll_print_int_list(dll_list_t *list);

void dll_print_string_list(dll_list_t *list);

#endif
