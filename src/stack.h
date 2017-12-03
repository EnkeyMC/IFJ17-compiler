/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_STACK_H
#define IFJ17_COMPILER_STACK_H

#include <stddef.h>
#include <stdbool.h>
#include "debug.h"

#define STACK_CHUNK 10

#ifndef DEREF_DATA
// Macro for easier and cleaner data dereferencing
#define DEREF_DATA(expr, type) *((type *) expr)
#endif // DEREF_DATA

typedef void (*stack_free_callback) (void* item);

typedef struct {
    int top;  /// Stack top
    int size;  /// Stack allocated size
    void** stack;  /// Dynamically allocated pointer array
} Stack;


/**
 * Allocate and initialize new stack
 * @param default_size default stack allocation size
 * @return pointer to new stack
 */
Stack* stack_init(int default_size);

/**
 * Return whether stack is empty or not
 * @param s valid Stack object
 * @return true if empty, false otherwise
 */
bool stack_empty(Stack* s);

/**
 * Return item on the top of the stack, does not remove it from the stack
 * @param s valid Stack object
 * @return item from top of the stack, NULL if stack is empty
 */
void* stack_top(Stack* s);

/**
 * Return item on the top of the stack and remove it from the stack
 * @param s valid Stack object
 * @return item from top of the stack, NULL if stack is empty
 */
void* stack_pop(Stack* s);

/**
 * Push item to a top of the stack
 * @param s valid Stack object
 * @param item to push to stack
 */
void stack_push(Stack* s, void* item);

/**
 * Free stack, calls free_item_f function on every item that is left in the stack if given
 * @param s valid Stack object
 * @param free_item_f function that frees item, can be NULL
 */
void stack_free(Stack* s, stack_free_callback free_item_f);

/**
 * Print debug info about Stack
 * @param s Stack
 * @param func debug function for stack items
 */
void stack_debug(Stack* s, debug_func func);

#endif //IFJ17_COMPILER_STACK_H
