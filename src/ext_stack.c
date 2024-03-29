/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <stdlib.h>
#include <assert.h>

#include "ext_stack.h"
#include "expr_grammar.h"
#include "error_code.h"
#include "debug.h"
#include "memory_manager.h"

ExtStack* ext_stack_init() {
	ExtStack* s = dllist_init(ext_stack_item_free);

	// Right after init push '$' on the stack
	ext_stack_push(s, EXPR_END_MARKER, NULL);
	return s;
}

bool ext_stack_expr_on_top(ExtStack *s) {
	dllist_activate_first(s);
	stack_item* item = (stack_item*) dllist_get_active(s);
	if (item->type_id == NT_EXPRESSION)
		return true;
	return false;
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

void ext_stack_shift(ExtStack* s, Token* token) {
	assert(token != NULL);
	dllist_activate_first(s);	
	stack_item* item = (stack_item*) dllist_get_active(s);

	while (item->type_id == NT_EXPRESSION || item->type_id == NT_LIST) {
		dllist_succ(s);
		item = (stack_item*) dllist_get_active(s);
	}

	// allocate memory for handle marker
	stack_item* handle_marker = (stack_item *) mm_malloc(sizeof(stack_item));
	handle_marker->type_id = EXPR_HANDLE_MARKER;
	handle_marker->token = NULL;

	// insert handle marker right after topmost terminal found in the previous loop
	dllist_pre_insert(s, handle_marker);

	// Allocate memory for token
	stack_item* token_ptr = (stack_item*) mm_malloc(sizeof(stack_item));
	token_ptr->type_id = token->id;
	token_ptr->token = token_copy(token);

	// finally insert token on the top of the stack (top == FIRST)
	dllist_insert_first(s, token_ptr);
}

/**
 * Find rule whose RHS can be found on stack between the top and EXPR_HANDLE_MARKER
 * @param s Stack to work with
 * @return LHS(non_terminal) of a rule on success, else EXPR_ERROR
 */
static Rule* find_rule(ExtStack* s) {
	dllist_activate_first(s);

	stack_item* item;
	for (int i = 0; i < NUM_OF_EXPR_RULES; i++) {
		int k;
		for (k = 0; expr_grammar.rules[i]->production[k] != END_OF_RULE; k++) {
			item = (stack_item*) dllist_get_active(s);
			if(item->type_id != expr_grammar.rules[i]->production[k])
				break;

			dllist_succ(s);
		}

		item = (stack_item*) dllist_get_active(s);
		if (item->type_id == EXPR_HANDLE_MARKER && expr_grammar.rules[i]->production[k] == END_OF_RULE)
			return expr_grammar.rules[i];	// rule FOUND

		dllist_activate_first(s);
	}

	return NULL;
}

int ext_stack_reduce(ExtStack* s, Parser* parser) {
	Rule* rule = find_rule(s);
	if (rule == NULL)
		return EXIT_SYNTAX_ERROR;
	else {
		// Init semantic analyzer for rule
		SemAnalyzer* sem_an = NULL;
		SemAnalyzer* sem_an_top = NULL;

		if (rule->sem_action != NULL) {
			sem_an = sem_an_init(rule->sem_action);
		}

		int ret_val = EXIT_SUCCESS;
		stack_item* item = (stack_item*) dllist_delete_first(s);
		while (item->type_id != EXPR_HANDLE_MARKER && ret_val == EXIT_SUCCESS) {
			if (sem_an != NULL) {  // Handle semantics
				if (IS_TOKEN(item->type_id)) {
					ret_val = sem_an->sem_action(sem_an, parser, SEM_VALUE_TOKEN(item->token));
				} else if (IS_NONTERMINAL(item->type_id)){
					sem_an_top = (SemAnalyzer*) sem_stack_pop(parser->sem_an_stack);
					assert(sem_an_top != NULL);

					// Pass value to current semantic action
					ret_val = sem_an->sem_action(sem_an, parser, *sem_an_top->value);
					sem_an_free(sem_an_top);
				}
			}
			ext_stack_item_free(item);
			item = (stack_item*) dllist_delete_first(s);
		}
		// Free also item containing handle marker
		ext_stack_item_free(item);

		if (ret_val != EXIT_SUCCESS)
			return ret_val;

		// Push current semantic analyzer on stack
		if (sem_an != NULL) {
			sem_stack_push(parser->sem_an_stack, sem_an);
		}

		// Push non_terminal found by find_rule()
		ext_stack_push(s, rule->for_nt, NULL);
		return EXIT_SUCCESS;
	}
}

void ext_stack_push(ExtStack* s, unsigned type_id, Token* token) {
	stack_item* item = (stack_item*) mm_malloc(sizeof(stack_item));
	item->type_id = type_id;
	item->token = token_copy(token);

	// Top of the stack is the first item in the list
	dllist_insert_first(s, item);
}

void ext_stack_item_free(void* item) {
	token_free(((stack_item*) item)->token);
	mm_free(item);
}

void ext_stack_free(ExtStack* s) {
	dllist_free(s);
}

void stack_item_debug(void* item) {
	stack_item* sitem = (stack_item*) item;

	debug("stack_item@%p: {", sitem);

	if (sitem != NULL) {
		debug(".type_id = %d, .token = ", sitem->type_id);
		token_debug(sitem->token);
	}

	debugs("}");
}

void ext_stack_debug(void* s) {
	ExtStack* stack = (ExtStack*) s;

	debug("ExtStack@%p: From top {\n", stack);

	if (stack != NULL) {
		dllist_activate_first(stack);
		while (dllist_active(stack)) {
			debugs("\t");
			stack_item_debug(dllist_get_active(stack));
			debugs("\n");
		}
	}

	debugs("}\n\n");
}
