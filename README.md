# Segregated Free Lists

This project implements a memory allocator using **segregated free lists**. The operations include INIT_HEAP, MALLOC, FREE, READ, WRITE, DUMP_MEMORY, and DESTROY_HEAP.

### INIT_HEAP
Initializes the heap structure (sfl), containing *num_lists* lists, each with a *data_size* as a power of 2, ranging from [2^3, 2^(num_lists + 3)]. Each node stores its address relative to the heap’s *start_address*.

### MALLOC
Allocates memory for a block of *data_size* bytes:

* Finds the smallest list in sfl with *data_size* >= *bytes*.
* If none is found, returns an Out of Memory error.
* If an exact match is found, the node is moved to allocated_blocks.
* If *data_size* > *bytes*, the node is split:
	* A new node for bytes is created and added to allocated_blocks.
	* The remaining portion is stored back in sfl.

### FREE
* If the address is 0x0, the operation does nothing (free(NULL) equivalent).
* Searches for the node in *allocated_blocks*; if not found, returns an *Invalid free* error.
* If found, the node is removed and re-added to sfl based on its reconstruction type.

### READ
* If address doesn't have *bytes* consecutive allocated blocks, returns a Segmentation fault.
* Otherwise, reads and displays *bytes* of data from the node.

### WRITE
* If address doesn't have bytes consecutive allocated blocks, returns a Segmentation fault.
* Otherwise, writes the string to the nodes, fragmented based on the *data_size* of each node.

### DUMP_MEMORY
Displays the current memory status, showing:

* Free and allocated blocks, their addresses.
* Statistics such as number of malloc and free calls, fragmentations, total memory, and memory usage.

### DESTROY_HEAP
Frees all allocated memory within the heap (sfl) and *allocated_blocks*.
