/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <assert.h>
#include "stack.h"
#include "memory_manager.h"


static void stack_realloc(Stack* s, int size) {
	// If size is same, no need to reallocate
	if (size == s->size)
		return;

	// Reallocate stack to new size
	s->stack = (void**) mm_realloc(s->stack, sizeof(void*) * size);
	// Set new size
	s->size = size;
}

Stack* stack_init(int size) {
	// Allocate new Stack
	Stack* s = (Stack*) mm_malloc(sizeof(Stack));

	// Allocate stack
	s->stack = (void**) mm_malloc(sizeof(void*) * size);

	s->size = size;
	s->top = -1;

	return s;
}

bool stack_empty(Stack* s) {
	assert(s != NULL);
	return s->top == -1;
}

void* stack_top(Stack* s) {
	assert(s != NULL);

	if (stack_empty(s))
		return NULL;

	return s->stack[s->top];
}

void* stack_pop(Stack* s) {
	assert(s != NULL);

	if (stack_empty(s))
		return NULL;

	// Return item and decrement top (pop it)
	return s->stack[s->top--];
}

void stack_push(Stack* s, void* item) {
	assert(s != NULL);

	// If the stack is full, reallocate to new size
	if (s->top + 1 >= s->size) {
		stack_realloc(s, s->size + STACK_CHUNK);
	}

	// Increment top and asign new item
	s->stack[++s->top] = item;
}

void stack_free(Stack* s, stack_free_callback free_item_f) {
	assert(s != NULL);

	// If  stack is not empty and free function was given,
	// free the remaining items
	if (!stack_empty(s) && free_item_f != NULL) {
		for (int i = 0; i <= s->top; ++i) {
			free_item_f(s->stack[i]);
		}
	}

	if (s->stack != NULL)
		mm_free(s->stack);
	mm_free(s);
}

void stack_debug(Stack* s, debug_func func) {
	debug("Stack@%p: From top {\n", s);

	if (s != NULL) {
		for (int i = s->top; i >= 0; --i) {
			debugs("\t");
			func(s->stack[i]);
			debugs("\n");
		}
	}

	debugs("}\n\n");
}
