#ifndef IFJ17_COMPILER_EXT_STACK_H
#define IFJ17_COMPILER_EXT_STACK_H

#include <stdbool.h>

#include "token.h"
#include "dllist.h"


/**
 * Extended stack data type
 */
typedef DLList ExtStack;

/**
 * Extended stack ITEM data type
 */
typedef struct stack_item_t {
	unsigned type_id;	/// TOKEN or non_terminal or END_MARKER
} stack_item;

/**
 * Init extended stack
 */
ExtStack* ext_stack_init();

/**
 * Free extended stack
 */
void ext_stack_free(ExtStack* s);

/**
 * Push TOKEN or EXPR_END_MARKER on top of the stack
 * @param s stack to work with
 * @param type_id symbol to be pushed
 * @return true on succes, false if memory allocation fails
 */
bool ext_stack_push(ExtStack* s, unsigned type_id);

/**
 * Get topmost TERMINAL on the stack
 * @param s stack to work with
 * @return value of the topmost TERMINAL
 */
unsigned ext_stack_top(ExtStack* s);

/**
 * Insert "EXPR_SHIFT" mark right after topmost TERMINAL on stack.
 * Push given token on top of the stack.
 * @param s stack to work with
 * @param token token to be instrted on top of the stack
 * @return true on success, false if memory allocation fails
 */
bool ext_stack_shift(ExtStack* s, token_e token);

/**
 * Try to reduce top of the stack till first EXPR_SHIFT mark on the stack
 * @param s stack to work with
 * @return true on success, false if top of the stack cannot be reduced
 */
bool ext_stack_reduce(ExtStack* s);

#endif //IFJ17_COMPILER_EXT_STACK_H
