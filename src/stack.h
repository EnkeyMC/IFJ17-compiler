#ifndef IFJ17_COMPILER_STACK_H
#define IFJ17_COMPILER_STACK_H

#include <stddef.h>
#include <stdbool.h>

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
 * @return pointer to new stack, NULL on allocation error
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
 * @return true on success, false on allocation error
 */
bool stack_push(Stack* s, void* item);

/**
 * Free stack, calls free_item_f function on every item that is left in the stack if given
 * @param s valid Stack object
 * @param free_item_f function that frees item, can be NULL
 */
void stack_free(Stack* s, stack_free_callback free_item_f);

#endif //IFJ17_COMPILER_STACK_H
