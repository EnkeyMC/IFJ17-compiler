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
#include "scanner.h"
#include "error_code.h"
#include "stack.h"

int parse() {
	int ret_code = EXIT_SUCCESS;

	HashTable* symtab_global = htab_init(0);
	if (symtab_global == NULL)
		return EXIT_INTERN_ERROR;

	Scanner* scanner = scanner_init();
	if (scanner == NULL) {
		htab_free(symtab_global);
		return EXIT_INTERN_ERROR;
	}

	if (!grammar_init()) {
		htab_free(symtab_global);
		scanner_free(scanner);
		return EXIT_INTERN_ERROR;
	}

	Stack* dtree_stack = stack_init(30);  // Stack for simulating syntax derivation tree
	non_terminal_e start_non_terminal = NT_LINE;
	stack_push(dtree_stack, &start_non_terminal);

	Token* token = NULL;
	int rule_idx;
	Rule* rule;

	do {
		token_free(token);

		token = scanner_get_token(scanner);
		if (token == NULL) {
			ret_code = EXIT_INTERN_ERROR;
			break;
		} else if (token->id == LEX_ERROR) {
			ret_code = EXIT_LEX_ERROR;
			break;
		}

		unsigned int s_top = *(unsigned int*) stack_top(dtree_stack);

		while (s_top < TERMINALS_START) {
			rule_idx = sparse_table_get(grammar.LL_table, s_top, get_token_column_value(token->id));

			rule = grammar.rules[rule_idx];

			if (rule == NULL) {
				ret_code = EXIT_SYNTAX_ERROR;
				break;
			}

			stack_pop(dtree_stack);

			for (int i = 0; rule->production[i] != END_OF_RULE; i++) {
				stack_push(dtree_stack, &rule->production[i]);
			}

			s_top = *(unsigned int*) stack_top(dtree_stack);
		}

		if (s_top == token->id && ret_code == EXIT_SUCCESS) {
			stack_pop(dtree_stack);
		} else {
			ret_code = EXIT_SYNTAX_ERROR;
			break;
		}
	} while (token->id != TOKEN_EOF);

	token_free(token);

	scanner_free(scanner);
	htab_free(symtab_global);
	grammar_free();
	stack_free(dtree_stack, NULL);

	return ret_code;
}
