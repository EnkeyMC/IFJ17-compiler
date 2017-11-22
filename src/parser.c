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
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "error_code.h"
#include "expr_grammar.h"
#include "expr_parser.h"
#include "3ac.h"
#include "buffer.h"


/**
 * Add built-in functions to HashTable
 * @param htab Hash table that stores function entries
 * @return true on success, false otherwise
 */
static bool add_built_ins(HashTable* htab) {
	const char *built_in_func[] = { "length", "substr", "asc", "chr" };

	htab_item* built_in;
	for (int i = 0; i < 4; i++) {
		built_in = htab_func_lookup(htab, built_in_func[i]);
		if (built_in == NULL) {
			htab_func_free(htab);
			return false;
		}

		func_set_def(built_in);
		// Set parameter types and return types
		switch (i) {
			// length
			case 0:
				func_set_rt(built_in, TOKEN_KW_INTEGER);
				if (! func_add_param(built_in, TOKEN_KW_STRING)) {
					htab_func_free(htab);
					return false;
				}
				break;
			// substr
			case 1:
				func_set_rt(built_in, TOKEN_KW_STRING);
				if (! func_add_param(built_in, TOKEN_KW_STRING)) {
					htab_func_free(htab);
					return false;
				}
				if (! func_add_param(built_in, TOKEN_KW_INTEGER)) {
					htab_func_free(htab);
					return false;
				}
				if (! func_add_param(built_in, TOKEN_KW_INTEGER)) {
					htab_func_free(htab);
					return false;
				}
				break;
			case 2:
				func_set_rt(built_in, TOKEN_KW_INTEGER);
				if (! func_add_param(built_in, TOKEN_KW_STRING)) {
					htab_func_free(htab);
					return false;
				}
				if (! func_add_param(built_in, TOKEN_KW_INTEGER)) {
					htab_func_free(htab);
					return false;
				}
				break;
			case 3:
				func_set_rt(built_in, TOKEN_KW_STRING);
				if (! func_add_param(built_in, TOKEN_KW_INTEGER)) {
					htab_func_free(htab);
					return false;
				}
			default: break;
		}
	}
/* TODO rewrite with new function calling convention
	// ASC function
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "asc"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "strlength"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_SUB, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)), false);
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "strlength"), addr_symbol(F_LOCAL, "str"), NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if1cond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "if1cond"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(0)), false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "wrongindex"), addr_symbol(F_LOCAL, "if1cond"), addr_constant(MAKE_TOKEN_BOOL(true)), false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if2cond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_GT, addr_symbol(F_LOCAL, "if2cond"), addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "strlength"), false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "wrongindex"), addr_symbol(F_LOCAL, "if2cond"), addr_constant(MAKE_TOKEN_BOOL(true)), false);
	IL_ADD(func_il, OP_STRI2INT, addr_symbol(F_LOCAL, "retval"), addr_symbol(F_LOCAL, "str"), addr_symbol(F_LOCAL, "i"), false);
	IL_ADD(func_il, OP_JUMP, addr_symbol("", "ascvalue"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "wrongindex"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "retval"), addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, false);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "ascvalue"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR, false);

	// SUBSTR function
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "substr"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "substr"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "anotherchar"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "counter"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "total"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "str"), NO_ADDR, false);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "substr"), addr_constant(MAKE_TOKEN_STRING("")), NO_ADDR, false);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "counter"), addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if1cond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "if1cond"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)), false);
	IL_ADD(func_il, OP_SUB, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)), false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "finalstring"), addr_symbol(F_LOCAL, "if1cond"), addr_constant(MAKE_TOKEN_BOOL(true)), false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if2cond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "if2cond"), addr_symbol(F_LOCAL, "n"), addr_constant(MAKE_TOKEN_INT(0)), false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "itoend"), addr_symbol(F_LOCAL, "if2cond"), addr_constant(MAKE_TOKEN_BOOL(true)), false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if3cond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_SUB, addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "i"), false);
	IL_ADD(func_il, OP_GT, addr_symbol(F_LOCAL, "if3cond"), addr_symbol(F_LOCAL, "n"), addr_symbol(F_LOCAL, "total"), false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "itoend"), addr_symbol(F_LOCAL, "if3cond"), addr_constant(MAKE_TOKEN_BOOL(true)), false);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "charloop"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "finalstring"), addr_symbol(F_LOCAL, "counter"), addr_symbol(F_LOCAL, "n"), false);
	IL_ADD(func_il, OP_GETCHAR, addr_symbol(F_LOCAL, "anotherchar"), addr_symbol(F_LOCAL, "str"), addr_symbol(F_LOCAL, "i"), false);
	IL_ADD(func_il, OP_CONCAT, addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "anotherchar"), false);
	IL_ADD(func_il, OP_ADD, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)), false);
	IL_ADD(func_il, OP_ADD, addr_symbol(F_LOCAL, "counter"), addr_symbol(F_LOCAL, "counter"), addr_constant(MAKE_TOKEN_INT(1)), false);
	IL_ADD(func_il, OP_JUMP, addr_symbol("", "charloop"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "itoend"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "str"), NO_ADDR, false);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "itoendwhilecond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "itoendwhilecond"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "itoendwhilecond"), addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "total"), false);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "finalstring"), addr_symbol(F_LOCAL, "itoendwhilecond"), addr_constant(MAKE_TOKEN_BOOL(false)), false);
	IL_ADD(func_il, OP_GETCHAR, addr_symbol(F_LOCAL, "anotherchar"), addr_symbol(F_LOCAL, "str"), addr_symbol(F_LOCAL, "i"), false);
	IL_ADD(func_il, OP_CONCAT, addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "anotherchar"), false);
	IL_ADD(func_il, OP_ADD, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)), false);
	IL_ADD(func_il, OP_JUMP, addr_symbol("", "itoendwhile"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "finalstring"), NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "retval"), addr_symbol(F_LOCAL, "substr"), NO_ADDR, false);
	IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR, false);
	IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR, false);
*/
	return true;
}

Parser* parser_init(Scanner* scanner) {
	Parser* parser = (Parser*) malloc(sizeof(Parser));
	if (parser == NULL)
		return NULL;

	parser->scanner = scanner;

	if (!grammar_init()) {
		free(parser);
		return NULL;
	}

	if (!expr_grammar_init()) {
		free(parser);
		grammar_free();
		return NULL;
	}

	parser->dtree_stack = stack_init(30);
	if (parser->dtree_stack == NULL) {
		free(parser);
		grammar_free();
		expr_grammar_free();
		return NULL;
	}

	parser->sym_tab_stack = dllist_init(htab_free);
	if (parser->sym_tab_stack == NULL) {
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}

	parser->sym_tab_global = htab_init(HTAB_INIT_SIZE);
	if (parser->sym_tab_global == NULL) {
		dllist_free(parser->sym_tab_stack);
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}

	parser->sym_tab_functions = htab_init(HTAB_INIT_SIZE);
	if (parser->sym_tab_functions == NULL) {
		htab_free(parser->sym_tab_global);
		dllist_free(parser->sym_tab_stack);
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}
	else if (! add_built_ins(parser->sym_tab_functions)) {
		htab_free(parser->sym_tab_global);
		dllist_free(parser->sym_tab_stack);
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}


	parser->sem_an_stack = dllist_init(sem_an_free);
	if (parser->sem_an_stack == NULL) {
		htab_func_free(parser->sym_tab_functions);
		htab_free(parser->sym_tab_global);
		dllist_free(parser->sym_tab_stack);
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}

	return parser;
}

void parser_free(Parser* parser) {
	assert(parser != NULL);

	htab_free(parser->sym_tab_global);
	dllist_free(parser->sym_tab_stack);
	dllist_free(parser->sem_an_stack);
	htab_func_free(parser->sym_tab_functions);
	grammar_free();
	expr_grammar_free();
	stack_free(parser->dtree_stack, NULL);
	free(parser);
}

int parse(Parser* parser) {
	assert(parser != NULL);

	int ret_code = EXIT_SUCCESS;

	// Push ending token and starting non terminal onto stack
	token_e eof_terminal = TOKEN_EOF;
	non_terminal_e start_non_terminal = NT_LINE;
	stack_push(parser->dtree_stack, &eof_terminal);
	stack_push(parser->dtree_stack, &start_non_terminal);

	Token* token = NULL;  // Temp var for current token
	int rule_idx;  // Temp var for rule index returned from LL table
	Rule* rule;  // Temp var for current rule
	unsigned int* s_top;  // Temp var for current stack top
	SemAnalyzer* sem_an, *sem_an_to_free;  // Temp vars for semantic analyzers

	// Start processing tokens
	do {  // Token loop
		token_free(token);  // Free last token, does nothing when token is NULL

		// Get next token from scanner

		token = scanner_get_token(parser->scanner);
		if (token == NULL) {  // Internal allocation error
			ret_code = EXIT_INTERN_ERROR;
		} else if (token->id == LEX_ERROR) {  // Lexical error
			ret_code = EXIT_LEX_ERROR;
		}

		// Look at what is on top of the stack
		s_top = (unsigned int*) stack_top(parser->dtree_stack);

		// If it is non terminal, rewrite it by rules, until there is terminal (token) in s_top
		while (*s_top < TERMINALS_START && ret_code == EXIT_SUCCESS) {  // Non terminal loop
			if (*s_top == NT_EXPRESSION) {  // If we are processing expression, call expression parser
				// Return token to buffer for expression parser
				scanner_unget_token(parser->scanner, token);
				// Call expression parser
				ret_code = parse_expression(parser);
				// Get new token
				token = scanner_get_token(parser->scanner);
				if (token == NULL) {  // Internal allocation error
					ret_code = EXIT_INTERN_ERROR;
				} else if (token->id == LEX_ERROR) {  // Lexical error
					ret_code = EXIT_LEX_ERROR;
				}

				stack_pop(parser->dtree_stack);
			} else {  // We are not processing expression
				// Look at LL table to get index to rule with right production
				rule_idx = sparse_table_get(grammar.LL_table, *s_top, get_token_column_value(token->id));

				// Get the rule from grammar
				rule = grammar.rules[rule_idx];

				// If no rule can be applied, return syntax error
				if (rule == NULL) {
					ret_code = EXIT_SYNTAX_ERROR;
				} else {  // Rule found
					if (rule->sem_action != NULL) {  // If rule has semantic action
						// Create semantic analyzer for the action and push it on stack
						sem_an = sem_an_init(rule->sem_action);
						if (sem_an == NULL) {
							ret_code = EXIT_INTERN_ERROR;
						} else {
							sem_stack_push(parser->sem_an_stack, sem_an);
						}
					}

					// Pop the current non terminal on top of stack
					stack_pop(parser->dtree_stack);

					// Rewrite it to the rule production (rule production is already reversed)
					for (int i = 0; rule->production[i] != END_OF_RULE; i++) {
						stack_push(parser->dtree_stack, &rule->production[i]);
					}
				}
			}
			// See what is now on top of the stack
			s_top = (unsigned int*) stack_top(parser->dtree_stack);
		}  // End non terminal loop

		if (ret_code == EXIT_SUCCESS) {
			// If the terminal (token) is the same as terminal on top of the stack and no error occurred, pop it from stack
			if (*s_top == token->id) {
				stack_pop(parser->dtree_stack);

				if (!sem_stack_empty(parser->sem_an_stack)) {
					// Handle semantics
					sem_an = (SemAnalyzer*) sem_stack_top(parser->sem_an_stack);
					ret_code = sem_an->sem_action(sem_an, parser, SEM_VALUE_TOKEN(token));

					// Finish up semantic analyzers
					while (sem_an != NULL && sem_an->finished && (ret_code == EXIT_SUCCESS)) {
						// If semantic action is finished, pop it from stack
						sem_stack_pop(parser->sem_an_stack);
						sem_an_to_free = sem_an;  // Store it for later freeing

						// Get parent semantic action
						sem_an = (SemAnalyzer*) sem_stack_top(parser->sem_an_stack);
						if (sem_an != NULL && sem_an_to_free->value != NULL) {
							// Call parent semantic action with value from child
							ret_code = sem_an->sem_action(sem_an, parser, *sem_an_to_free->value);
						}
						sem_an_free(sem_an_to_free);  // Free finished semantic analyzer
					}

					if (ret_code != EXIT_SUCCESS) {
						break;
					}
				}
			} else {
				// Else there is syntax error
				ret_code = EXIT_SYNTAX_ERROR;
				break;
			}
		} else {
			break;
		}
	} while (token == NULL || token->id != TOKEN_EOF);  // End token loop

	token_free(token);


	return ret_code;
}

char* generate_uid() {
	static unsigned int seed = 1;
	Buffer* buffer = buffer_init(5);
	if (buffer == NULL)
		return NULL;

	buffer_append_str(buffer, "ID");

	unsigned int num = seed;
	while (num) {
		buffer_append_c(buffer, (char) (num % 10 + '0'));
		num /= 10;
	}

	seed++;

	char* out = (char*) malloc(sizeof(char) * (strlen(buffer->str) + 1));
	if (out == NULL) {
		buffer_free(buffer);
		return NULL;
	}

	strcpy(out, buffer->str);
	buffer_free(buffer);
	return out;
}
