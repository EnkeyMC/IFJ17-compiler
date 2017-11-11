#include <stdlib.h>

#include "ext_stack.h"
#include "grammar.h"
#include "expr_grammar.h"

ExtStack* ext_stack_init() {
	ExtStack* s = dllist_init(NULL);
	if (s == NULL)
		return NULL;

	// Right after init push '$' on the stack
	if (ext_stack_push(s, EXPR_END_MARKER))
		return s;
	else
		return NULL;
}

unsigned ext_stack_top(ExtStack* s) {
	dllist_activate_first(s);	
	stack_item* item = (stack_item*) dllist_get_active(s);
	// item cannot be null - stack is never empty

	// true if current item is non_terminal
	while (item->type_id == NT_EXPRESSION || item->type_id == NT_LIST) {
		// There is always at least one TERMINAL on the stack
		dllist_succ(s);
		item = (stack_item*) dllist_get_active(s);
	}

	return item->type_id;
}

bool ext_stack_shift(ExtStack* s, token_e token) {
	dllist_activate_first(s);	
	stack_item* item = (stack_item*) dllist_get_active(s);

	while (item->type_id == NT_EXPRESSION || item->type_id == NT_LIST) {
		dllist_succ(s);
		item = (stack_item*) dllist_get_active(s);
	}

	// allocate memory for handle marker
	stack_item* handle_marker = (stack_item *) malloc(sizeof(stack_item));
	if (handle_marker == NULL)
		return false;
	handle_marker->type_id = EXPR_HANDLE_MARKER;

	// insert handle marker right after topmost terminal found in the previous loop
	if (! dllist_pre_insert(s, handle_marker)) {
		free(handle_marker);
		return false;
	}

	// Allocate memory for token
	stack_item* token_ptr = (stack_item*) malloc(sizeof(stack_item));
	if (token_ptr == NULL)
		return false;
	token_ptr->type_id = token;

	// finally insert token on the top of the stack (top == FIRST)
	return dllist_insert_first(s, token_ptr);
}

/**
 * Find rule whose RHS can be found on stack between the top and EXPR_HANDLE_MARKER
 * @param s Stack to work with
 * @return LHS(non_terminal) of a rule on success, else EXPR_ERROR
 */
static unsigned find_rule(ExtStack* s) {
	dllist_activate_first(s);

	int i;
	bool found = false;
	stack_item* item;
	for (i = 0; i < NUM_OF_EXPR_RULES; i++) {
		int k;
		for (k = 0; expr_grammar.rules[i]->production[k] != END_OF_RULE; k++) {
			item = (stack_item*) dllist_get_active(s);
			if(item->type_id != expr_grammar.rules[i]->production[k])
				break;

			dllist_succ(s);
		}

		item = (stack_item*) dllist_get_active(s);
		if (item->type_id == EXPR_HANDLE_MARKER && expr_grammar.rules[i]->production[k] == END_OF_RULE) {
			found = true;
			break;
		}

		dllist_activate_first(s);
	}

	if (found)
		return expr_grammar.rules[i]->for_nt;
	else
		return EXPR_ERROR;
}

bool ext_stack_reduce(ExtStack* s) {
	unsigned non_terminal = find_rule(s);
	if (non_terminal == EXPR_ERROR)
		return false;
	else {
		stack_item* item = (stack_item*) dllist_delete_first(s);
		while (item->type_id != EXPR_HANDLE_MARKER) {
			free(item);
			item = (stack_item*) dllist_delete_first(s);
		}
		// Free also item containing handle marker
		free(item);

		// Push non_terminal found by find_rule()
		if (ext_stack_push(s, non_terminal))
			return true;
		else
			return false;
	}
}

bool ext_stack_push(ExtStack* s, unsigned type_id) {
	stack_item* item = (stack_item*) malloc(sizeof(stack_item));
	if (item == NULL)
		return false;
	item->type_id = type_id;

	// Top of the stack is the first item in the list
	return dllist_insert_first(s, item);
}

void ext_stack_free(ExtStack* s) {
	dllist_free(s);
}
