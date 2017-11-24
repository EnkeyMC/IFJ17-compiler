/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_EXT_STACK_H
#define IFJ17_COMPILER_EXT_STACK_H

#include <stdbool.h>

#include "token.h"
#include "dllist.h"
#include "parser.h"


/**
 * Extended stack data type
 */
typedef DLList ExtStack;

/**
 * Extended stack ITEM data type
 */
typedef struct stack_item_t {
	unsigned type_id;	/// TOKEN or non_terminal or END_MARKER
	Token* token;  /// If type_id is token, here is stored Token data for semantic actions
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
 * Free stack item
 * @param item stack_item
 */
void ext_stack_item_free(void* item);

/**
 * Push TOKEN or EXPR_END_MARKER on top of the stack
 * @param s stack to work with
 * @param type_id symbol to be pushed
 * @param token if type_id is token, this is the token data, otherwise NULL
 * @return true on success, false if memory allocation fails
 */
bool ext_stack_push(ExtStack* s, unsigned type_id, Token* token);

/**
 * Get topmost TERMINAL on the stack
 * @param s stack to work with
 * @return value of the topmost TERMINAL
 */
unsigned ext_stack_top(ExtStack* s);

/**
 * True if topmost item on stack is expression
 * @param s stack to work with
 * @return true if expression was correctly reduced, false otherwise
 */
bool ext_stack_expr_on_top(ExtStack* s);

/**
 * Insert "EXPR_SHIFT" mark right after topmost TERMINAL on stack.
 * Push given token on top of the stack.
 * @param s stack to work with
 * @param token token to be inserted on top of the stack
 * @return true on success, false if memory allocation fails
 */
bool ext_stack_shift(ExtStack* s, Token* token);

/**
 * Try to reduce top of the stack till first EXPR_SHIFT mark on the stack
 * @param s stack to work with
 * @param parser Parser for semantic actions
 * @return true on success, false if top of the stack cannot be reduced
 */
int ext_stack_reduce(ExtStack* s, Parser* parser);

/**
 * Debug info about stack_item
 * @param item stack_item
 */
void stack_item_debug(void* item);

/**
 * Debug info about ExtStack
 * @param s ExtStack
 */
void ext_stack_debug(void* s);

#endif //IFJ17_COMPILER_EXT_STACK_H
