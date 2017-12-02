/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <stdlib.h>
#include <assert.h>

#include "memory_manager.h"
#include "error_code.h"
#include "debug.h"

#define MEMORY_CHUNK 100

typedef void* Block;  // Just a little bit of abstraction

/**
 * Memory Manager
 */
struct {
	Block* memory;  /// Array of allocated memory blocks
	unsigned first_free;  /// Index of first free Block in memory (always last unused Block)
	unsigned size;  /// Actual allocated size of memory
	unsigned max_blocks;  /// Max blocks allocated in the lifespan of memory manager
} mm;

/**
 * Swap two memory blocks
 * @param item1
 * @param item2
 */
static void memory_block_swap(Block *item1, Block *item2) {
	void* ptr = *item1;
	*item1 = *item2;
	*item2 = ptr;
}

/**
 * Expand Memory Manager memory for storing allocated blocks by MEMORY_CHUNK
 */
static void memory_expand() {
	mm.memory = (Block*) realloc(mm.memory, sizeof(Block) * (mm.size + MEMORY_CHUNK));
	if (mm.memory == NULL)
		exit(EXIT_INTERN_ERROR);

	mm.size += MEMORY_CHUNK;
}

/**
 * Get free memory block, expand memory if full
 * @return free memory block (considered used after call)
 */
static Block* get_free_memory_block() {
	if (mm.first_free >= mm.size - 1) {
		memory_expand();
	}

	if (mm.first_free + 1 > mm.max_blocks)
		mm.max_blocks = mm.first_free + 1;

	return &mm.memory[mm.first_free++];
}

/**
 * Find memory block with given ptr
 * @param ptr Allocated memory
 * @return memory block with given ptr
 */
static Block* memory_block_find(void *ptr) {
	for (unsigned i = 0; i < mm.first_free; ++i) {
		if (mm.memory[i] == ptr)
			return &mm.memory[i];
	}

	debugs("(Memory Manager) ERROR: Memory was not allocated or was already freed!");
	exit(EXIT_INTERN_ERROR);
}

// PUBLIC INTERFACE

void mem_manager_init() {
	mm.memory = (Block *) malloc(sizeof(Block) * MEMORY_CHUNK);
	if (mm.memory == NULL)
		exit(EXIT_INTERN_ERROR);

	mm.size = MEMORY_CHUNK;
	mm.first_free = 0;
	mm.max_blocks = 0;
}

void mem_manager_free() {
	debugs("\n======= Memory Manager =======\n");
	debug("In use at exit: %d blocks\n", mm.first_free);
	debug("Max blocks used: %d blocks\n", mm.max_blocks);
	debugs("==============================\n");

#ifndef MEM_MNG_NO_FREE
	for (; mm.first_free > 0; mm.first_free--) {
		free(mm.memory[mm.first_free - 1]);
	}
#endif

	free(mm.memory);
}

void* mm_malloc(size_t size) {
	Block* item = get_free_memory_block();

	*item = malloc(size);
	if (*item == NULL) {
		mem_manager_free();
		exit(EXIT_INTERN_ERROR);
	}

	return *item;
}

void* mm_realloc(void* ptr, size_t size) {
	Block* item = memory_block_find(ptr);

	*item = realloc(ptr, size);
	if (*item == NULL && size != 0) {
		mem_manager_free();
		exit(EXIT_INTERN_ERROR);
	}

	return *item;
}

void mm_free(void* ptr) {
	assert(ptr != NULL);
	Block * item = memory_block_find(ptr);
	free(*item);

	mm.first_free--;
	memory_block_swap(item, &mm.memory[mm.first_free]);
}
