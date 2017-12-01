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

typedef void* Block;

struct {
	Block* memory;
	unsigned first_free;
	unsigned size;
} mm;

static void memory_item_swap(Block* item1, Block* item2) {
	void* ptr = *item1;
	*item1 = *item2;
	*item2 = ptr;
}

static void memory_expand() {
	mm.memory = (Block*) realloc(mm.memory, sizeof(Block) * (mm.size + MEMORY_CHUNK));
	if (mm.memory == NULL)
		exit(EXIT_INTERN_ERROR);

	mm.size += MEMORY_CHUNK;
}

static Block* get_free_memory_item() {
	if (mm.first_free >= mm.size - 1) {
		memory_expand();
	}

	return &mm.memory[mm.first_free++];
}

static Block* memory_item_find(void* ptr) {
	for (unsigned i = 0; i < mm.first_free; ++i) {
		if (mm.memory[i] == ptr)
			return &mm.memory[i];
	}

	debugs("ERROR: Memory was not allocated or was already freed!");
	exit(EXIT_INTERN_ERROR);
}

// PUBLIC INTERFACE

void mem_manager_init() {
	mm.memory = (Block *) malloc(sizeof(Block) * MEMORY_CHUNK);
	if (mm.memory == NULL)
		exit(EXIT_INTERN_ERROR);

	mm.size = MEMORY_CHUNK;
	mm.first_free = 0;
}

void mem_manager_free() {
	debug("In use at exit: %d blocks\n", mm.first_free);

	for (; mm.first_free > 0; mm.first_free--) {
		free(mm.memory[mm.first_free - 1]);
	}

	free(mm.memory);
}

void* mm_malloc(size_t size) {
	Block* item = get_free_memory_item();

	*item = malloc(size);
	if (*item == NULL) {
		mem_manager_free();
		exit(EXIT_INTERN_ERROR);
	}

	return *item;
}

void* mm_realloc(void* ptr, size_t size) {
	Block * item = memory_item_find(ptr);

	*item = realloc(ptr, size);
	if (*item == NULL && size != 0) {
		mem_manager_free();
		exit(EXIT_INTERN_ERROR);
	}

	return *item;
}

void mm_free(void* ptr) {
	assert(ptr != NULL);
	Block * item = memory_item_find(ptr);

	mm.first_free--;
	memory_item_swap(item, &mm.memory[mm.first_free]);
}
