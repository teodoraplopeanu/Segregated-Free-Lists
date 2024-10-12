#include "about_sfls.h"

#define MAX_STRING 256

int min(int x, int y)
{
	if (x < y)
		return x;
	return y;
}

dll_list_t **init_heap(sfl_t *sfl)
{
	sfl->array = malloc(sfl->num_lists * sizeof(dll_list_t *));
	DIE(!sfl->array, "Memory allocation error\n");

	int dim = 8;

	sfl->malloc_calls = 0;
	sfl->fragmetations = 0;
	sfl->free_calls = 0;

	sfl->total_memory = sfl->num_lists * sfl->bytes_per_list;

	for (unsigned int i = 0; i < sfl->num_lists; ++i) {
		/* Create a list of data_size 2 ^ (i + 3) */
		sfl->array[i] = dll_create(dim);
		/* Set the starting address of the list */
		sfl->array[i]->start_address = sfl->start_address +
									i * sfl->bytes_per_list;

		for (unsigned int j = 0; j < sfl->bytes_per_list / dim; j++) {
			long address = sfl->array[i]->start_address +
							j * sfl->array[i]->data_size;
			dll_add_nth_node(sfl->array[i], sfl->array[i]->size, address);
		}

		/* Increase the dimension */
		dim *= 2;
	}

	return sfl->array;
}

void malloc_memory(sfl_t *sfl, unsigned int bytes,
				   allocated_blocks_t *allocated_blocks)
{
	int index_final = sfl->num_lists - 1;
	while (!sfl->array[index_final]->size)
		index_final--;

	if (sfl->array[index_final]->data_size < bytes) {
		printf("Out of memory\n");
		return;
	}

	sfl->malloc_calls++;

	/* Find the index of the list that contains the block to be allocated */
	unsigned int index = 0;
	while ((index < sfl->num_lists && sfl->array[index]->data_size <
			bytes) || sfl->array[index]->size == 0)
		index++;
	/*while (index < sfl->num_lists && sfl->array[index]->data_size < bytes)
		index++;
	while (index < sfl->num_lists && sfl->array[index]->size == 0)
		index++;*/

	/* That's the node I'm working with */
	dll_node_t *new_node = dll_remove_nth_node(sfl->array[index], 0);

	/* If the found block has exactly the number of bytes that we need */
	if (sfl->array[index]->data_size == bytes) {
		add_node_to_ab(new_node, allocated_blocks, bytes);
	} else {
		/* The size of the block is greater than bytes */

		/* Fragment the node */
		sfl->fragmetations++;

		dll_node_t *node1 = malloc(sizeof(dll_node_t));
		DIE(!node1, "Memory allocation error\n");
		node1->data = malloc(sizeof(sfl_info_t));
		((sfl_info_t *)node1->data)->address =
		((sfl_info_t *)new_node->data)->address;
		add_node_to_ab(node1, allocated_blocks, bytes);
		free(node1->data);
		free(node1);

		dll_node_t *node2 = malloc(sizeof(dll_node_t));
		DIE(!node2, "Memory allocation error\n");
		node2->data = malloc(sizeof(sfl_info_t));
		((sfl_info_t *)node2->data)->address =
		((sfl_info_t *)new_node->data)->address + bytes;

		add_node_to_sfl(node2, sfl, sfl->array[index]->data_size - bytes);
		free(node2->data);
		free(node2);
	}
	free(new_node->data);
	free(new_node);
}

void dump_memory(sfl_t *sfl, allocated_blocks_t *allocated_blocks)
{
	printf("+++++DUMP+++++\n");

	printf("Total memory: %d bytes\n", sfl->total_memory);

	int allocated_mem = allocated_memory(allocated_blocks);
	printf("Total allocated memory: %d bytes\n", allocated_mem);

	printf("Total free memory: %d bytes\n", sfl->total_memory - allocated_mem);

	int num_fb = num_free_blocks(sfl);
	printf("Free blocks: %d\n", num_fb);

	int num_ab = num_allocated_blocks(allocated_blocks);
	printf("Number of allocated blocks: %d\n", num_ab);

	printf("Number of malloc calls: %d\n", sfl->malloc_calls);

	printf("Number of fragmentations: %d\n", sfl->fragmetations);

	printf("Number of free calls: %d\n", sfl->free_calls);

	for (unsigned int i = 0; i < sfl->num_lists; i++) {
		if (sfl->array[i]->size) {
			printf("Blocks with %d bytes - %d free block(s) :",
				   sfl->array[i]->data_size, sfl->array[i]->size);

			dll_node_t *current_node = sfl->array[i]->head;

			for (unsigned int j = 0; j < sfl->array[i]->size; j++) {
				printf(" 0x%lx", ((sfl_info_t *)current_node->data)->address);
				current_node = current_node->next;
			}
			printf("\n");
		}
	}

	printf("Allocated blocks :");

	ll_node_t *current_node_ab = allocated_blocks->list->head;

	//for (unsigned int i = 0; i < allocated_blocks->size; i++) {
	while (current_node_ab) {
		printf(" (0x%lx - %d)", ((ab_info_t *)current_node_ab->data)->address,
			   ((ab_info_t *)current_node_ab->data)->data_size);
		current_node_ab = current_node_ab->next;
	}
	printf("\n");

	printf("-----DUMP-----\n");
}

void destroy_heap(sfl_t *sfl, allocated_blocks_t *allocated_blocks);

void read(sfl_t *sfl, allocated_blocks_t *allocated_blocks, long address,
		  int bytes)
{
	if (seg_fault(allocated_blocks, address, bytes)) {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(sfl, allocated_blocks);
		destroy_heap(sfl, allocated_blocks);
		exit(0);
	}

	/* Afisare */
	ll_node_t *current_node = allocated_blocks->list->head;

	while (!(address >= ((ab_info_t *)current_node->data)->address &&
		    address <= ((ab_info_t *)current_node->data)->address +
		   ((ab_info_t *)current_node->data)->data_size))
		current_node = current_node->next;

	int index_start = address - ((ab_info_t *)current_node->data)->address;

	while (bytes > 0) {
		for (int i = index_start; i <
		min(bytes, ((ab_info_t *)current_node->data)->data_size); i++)
			printf("%c",
				   ((char *)(((ab_info_t *)current_node->data)->data))[i]);

		bytes -= ((ab_info_t *)current_node->data)->data_size;
		current_node = current_node->next;
	}

	printf("\n");
}

void write(sfl_t *sfl, allocated_blocks_t *allocated_blocks,
		   long address, char *string, int bytes)
{
	if (bytes > (int)(strlen(string)))
		bytes = strlen(string);

	if (seg_fault(allocated_blocks, address, bytes) == 1) {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(sfl, allocated_blocks);
		destroy_heap(sfl, allocated_blocks);
		exit(0);
		return;
	}

	ll_node_t *current_node = allocated_blocks->list->head;

	while (((ab_info_t *)current_node->data)->address != address)
		current_node = current_node->next;

	while (bytes > 0) {
		int number = min(bytes, ((ab_info_t *)current_node->data)->data_size);
		memcpy(((ab_info_t *)current_node->data)->data, string, number);
		//strcpy(string, string + number);
		char *buffer = malloc(MAX_STRING);
		DIE(!buffer, "Memory allocation error\n");
		strcpy(buffer, string + number);
		strcpy(string, buffer);
		free(buffer);
		string[strlen(string)] = '\0';
		bytes -= ((ab_info_t *)current_node->data)->data_size;
		current_node = current_node->next;
	}
}

void free_address(long address, sfl_t *sfl,
				  allocated_blocks_t *allocated_blocks)
{
	if (address == 0)
		return;

	ll_node_t *current_node = allocated_blocks->list->head;
	unsigned int index = 0;

	while (current_node && ((ab_info_t *)current_node->data)->address
		!= address) {
		index++;
		current_node = current_node->next;
	}

	if (!current_node) {
		printf("Invalid free\n");
		return;
	}

	sfl->free_calls++;

	ll_node_t *removed_node = ll_remove_nth_node(allocated_blocks->list,
												index);

	dll_node_t *new_node = malloc(sizeof(dll_node_t));
	new_node->data = malloc(sizeof(sfl_info_t));
	((sfl_info_t *)new_node->data)->address =
	((ab_info_t *)removed_node->data)->address;

	add_node_to_sfl(new_node, sfl,
					((ab_info_t *)removed_node->data)->data_size);
	free(((ab_info_t *)removed_node->data)->data);
	free(removed_node->data);
	free(removed_node);
	free(new_node->data);
	free(new_node);
}

void destroy_heap(sfl_t *sfl, allocated_blocks_t *allocated_blocks)
{
	ll_free(&allocated_blocks->list);
	free(allocated_blocks);

	for (unsigned int i = 0; i < sfl->num_lists; i++)
		dll_free(&sfl->array[i]);

	free(sfl->array);
	free(sfl);
}

int main(void)
{
	char command[MAX_STRING];

	sfl_t *sfl;
	allocated_blocks_t *allocated_blocks = malloc(sizeof(allocated_blocks_t));
	allocated_blocks->list = ll_create();
	allocated_blocks->size = 0;

	while (scanf("%s", command)) {
		if (strcmp(command, "INIT_HEAP") == 0) {
			sfl = malloc(sizeof(sfl_t));
			DIE(!sfl, "Memory allocation error\n");

			scanf("%lx%d%d%d", &sfl->start_address, &sfl->num_lists,
				  &sfl->bytes_per_list, &sfl->reconst_type);

			sfl->array = init_heap(sfl);
		}

		if (strcmp(command, "MALLOC") == 0) {
			unsigned int bytes;
			scanf("%d", &bytes);
			malloc_memory(sfl, bytes, allocated_blocks);
		}

		if (strcmp(command, "FREE") == 0) {
			long address;
			scanf("%lx", &address);
			free_address(address, sfl, allocated_blocks);
		}

		if (strcmp(command, "READ") == 0) {
			long address;
			int bytes;
			scanf("%lx%d", &address, &bytes);
			read(sfl, allocated_blocks, address, bytes);
		}

		if (strcmp(command, "WRITE") == 0) {
			long address;
			int bytes;
			scanf("%lx", &address);

			char *buffer = calloc(256, sizeof(char));
			DIE(!buffer, "Memory allocation error\n");

			char c;
			int index = 0;
			scanf(" \"");

			while (scanf("%c", &c) == 1 && c != '"')
				buffer[index++] = c;

			buffer[index] = '\0';

			scanf("%d", &bytes);

			write(sfl, allocated_blocks, address, buffer, bytes);
			free(buffer);
		}

		if (strcmp(command, "DUMP_MEMORY") == 0)
			dump_memory(sfl, allocated_blocks);

		if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy_heap(sfl, allocated_blocks);
			break;
		}
	}

	return 0;
}
