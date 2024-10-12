#include "about_sfls.h"

void add_node_to_ab(dll_node_t *new_node, allocated_blocks_t *allocated_blocks,
					unsigned int bytes)
{
	int index = 0;

	if (allocated_blocks->list->size == 0) {
		ll_add_nth_node(allocated_blocks->list, 0,
					    bytes, ((ab_info_t *)new_node->data)->address);
		return;
	}

	ll_node_t *current_node = allocated_blocks->list->head;

	while (current_node && ((ab_info_t *)current_node->data)->address <
		   ((ab_info_t *)new_node->data)->address) {
		index++;
		current_node = current_node->next;
	}

	ll_add_nth_node(allocated_blocks->list, index, bytes,
					((ab_info_t *)new_node->data)->address);
	allocated_blocks->size++;
}

void add_node_to_sfl(dll_node_t *new_node, sfl_t *sfl, unsigned int bytes)
{
	unsigned int index = 0;

	while (sfl->array[index]->data_size < bytes && index < sfl->num_lists)
		index++;

	/* Don't have an allocated list for blocks of size bytes */
		if (sfl->array[index]->data_size != bytes) {
			/* Create a list of data_size bytes */

			sfl->array = realloc(sfl->array, (sfl->num_lists + 1) *
											sizeof(dll_list_t *));

			/* Shift the lists */
			for (unsigned int i = sfl->num_lists; i > index; i--)
				sfl->array[i] = sfl->array[i - 1];

			/* Create the new list */
			sfl->array[index] = dll_create(bytes);
			sfl->num_lists++;

			/* Add the block */
			dll_add_nth_node(sfl->array[index], 0,
							 ((sfl_info_t *)new_node->data)->address);
		} else {
			/* Already have an allocated list for blocks of size bytes */

			/* Search for the position in ascending order of addresses */
			if (sfl->array[index]->size == 0) {
				dll_add_nth_node(sfl->array[index], 0,
								 ((sfl_info_t *)new_node->data)->address);
			} else {
				long position = 0;

				dll_node_t *current_node = sfl->array[index]->head;

				while (current_node &&
					   ((sfl_info_t *)current_node->data)->address <
					   ((sfl_info_t *)new_node->data)->address && position <
					   sfl->array[index]->size) {
					position++;
					current_node = current_node->next;
				}

				dll_add_nth_node(sfl->array[index], position,
								 ((sfl_info_t *)new_node->data)->address);
			}
		}
}

int allocated_memory(allocated_blocks_t *allocated_blocks)
{
	ll_node_t *current_node = allocated_blocks->list->head;
	int sum = 0;

	while (current_node) {
		sum += ((ab_info_t *)current_node->data)->data_size;
		current_node = current_node->next;
	}

	return sum;
}

int num_free_blocks(sfl_t *sfl)
{
	int cnt = 0;
	for (unsigned int i = 0; i < sfl->num_lists; i++) {
		dll_node_t *current_node = sfl->array[i]->head;

		for (unsigned int j = 0; j < sfl->array[i]->size; ++j) {
			cnt++;
			current_node = current_node->next;
		}
	}
	return cnt;
}

int num_allocated_blocks(allocated_blocks_t *allocated_blocks)
{
	int cnt = 0;

	ll_node_t *current_node = allocated_blocks->list->head;

	while (current_node) {
		cnt++;
		current_node = current_node->next;
	}

	return cnt;
}

int seg_fault(allocated_blocks_t *allocated_blocks, long address, int bytes)
{
	ll_node_t *current_node = allocated_blocks->list->head;
	unsigned int index = 0;

    // Find the node with the specified address
	while (current_node &&
		   !(address >= ((ab_info_t *)current_node->data)->address && address <=
		   ((ab_info_t *)current_node->data)->address +
		   ((ab_info_t *)current_node->data)->data_size)) {
		index++;
		current_node = current_node->next;
	}

	// If the address is not found in allocated blocks, return 1
	// (segmentation fault)
	if (!current_node)
		return 1;

    // Check if accessing bytes starting from the address
	// would cause segmentation fault
	while (bytes > 0) {
		if (!current_node ||
		    (bytes > ((ab_info_t *)current_node->data)->data_size &&
			current_node->next &&
			((ab_info_t *)current_node->next->data)->address !=
			((ab_info_t *)current_node->data)->address +
			((ab_info_t *)current_node->data)->data_size))
			return 1;
		bytes -= ((ab_info_t *)current_node->data)->data_size;
		current_node = current_node->next;
	}

	return 0; // No segmentation fault detected
}
