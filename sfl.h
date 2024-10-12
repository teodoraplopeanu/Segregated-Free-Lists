#ifndef ABOUT_SFLS_H
#define ABOUT_SFLS_H

#include "about_dlls.h"
#include "about_lls.h"
#include <string.h>

typedef struct sfl_t {
	dll_list_t **array;
	long start_address;
	unsigned int num_lists;
	unsigned int bytes_per_list;
	unsigned int reconst_type;
	int malloc_calls;
	int fragmetations;
	int free_calls;
	int total_memory;
} sfl_t;

typedef struct allocated_blocks_t {
	ll_list_t *list;
	unsigned int size;
} allocated_blocks_t;

void add_node_to_ab(dll_node_t *new_node, allocated_blocks_t *allocated_blocks,
					unsigned int bytes);

void add_node_to_sfl(dll_node_t *new_node, sfl_t *sfl, unsigned int bytes);

int allocated_memory(allocated_blocks_t *allocated_blocks);

int num_free_blocks(sfl_t *sfl);

int num_allocated_blocks(allocated_blocks_t *allocated_blocks);

int seg_fault(allocated_blocks_t *allocated_blocks, long address, int bytes);

char *read_between_qm(void);

#endif
