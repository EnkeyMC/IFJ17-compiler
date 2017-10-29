#include <malloc.h>
#include <assert.h>
#include "stack.h"


static bool stack_realloc(Stack* s, int size) {
	// If size is same, no need to reallocate
	if (size == s->size)
		return true;

	// Reallocate stack to new size
	s->stack = (void**) realloc(s->stack, sizeof(void*) * size);
	if (s->stack == NULL) { // Check reallocation success
		s->size = 0;
		s->top = -1;
		return false;
	}
	// Set new size
	s->size = size;

	return true;
}

Stack* stack_init(int size) {
	// Allocate new Stack
	Stack* s = (Stack*) malloc(sizeof(Stack));
	if (s == NULL)
		return NULL;

	// Allocate stack
	s->stack = (void**) malloc(sizeof(void*) * size);
	if (s->stack == NULL) {
		free(s);
		return NULL;
	}

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

bool stack_push(Stack* s, void* item) {
	assert(s != NULL);

	// If the stack is full, reallocate to new size
	if (s->top + 1 >= s->size) {
		if (!stack_realloc(s, s->size + STACK_CHUNK)) {
			return false;
		}
	}

	// Increment top and asign new item
	s->stack[++s->top] = item;
	return true;
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
		free(s->stack);
	free(s);
}
