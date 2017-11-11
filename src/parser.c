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
#include "parser.h"
#include "error_code.h"


Parser* parser_init(Scanner* scanner) {
	Parser* parser = (Parser*) malloc(sizeof(Parser));
	if (parser == NULL)
		return NULL;

	parser->scanner = scanner;

	if (!grammar_init()) {
		free(parser);
		return NULL;
	}

	parser->dtree_stack = stack_init(30);
	if (parser->dtree_stack == NULL) {
		free(parser);
		grammar_free();
		return NULL;
	}

	return parser;
}

void parser_free(Parser* parser) {
	assert(parser != NULL);

	grammar_free();
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

	// Start processing tokens
	do {  // Token loop
		token_free(token);  // Free last token, does nothing first iteration

		// Get next token from scanner
		token = scanner_get_token(parser->scanner);
		if (token == NULL) {  // Internal allocation error
			ret_code = EXIT_INTERN_ERROR;
			break;
		} else if (token->id == LEX_ERROR) {  // Lexical error
			ret_code = EXIT_LEX_ERROR;
			break;
		}

		// Look at what is on top of the stack
		s_top = (unsigned int*) stack_top(parser->dtree_stack);

		// If it is non terminal, rewrite it by rules, until there is terminal (token) in s_top
		while (*s_top < TERMINALS_START) {  // Non terminal loop
			// Look at LL table to get index to rule with right production
			rule_idx = sparse_table_get(grammar.LL_table, *s_top, get_token_column_value(token->id));

			// Get the rule from grammar
			rule = grammar.rules[rule_idx];

			// If no rule can be applied, return syntax error
			if (rule == NULL) {
				ret_code = EXIT_SYNTAX_ERROR;
				break;
			}

			// Pop the current non terminal on top of stack
			stack_pop(parser->dtree_stack);

			// Rewrite it to the rule production (rule production is already reversed)
			for (int i = 0; rule->production[i] != END_OF_RULE; i++) {
				stack_push(parser->dtree_stack, &rule->production[i]);
			}

			// See what is now on top of the stack
			s_top = (unsigned int*) stack_top(parser->dtree_stack);
		}  // End non terminal loop

		// If the terminal (token) is the same as terminal on top of the stack and no error occured, pop it from stack
		if (*s_top == token->id && ret_code == EXIT_SUCCESS) {
			stack_pop(parser->dtree_stack);
		} else {
			// Else there is syntax error
			ret_code = EXIT_SYNTAX_ERROR;
			break;
		}
	} while (token->id != TOKEN_EOF);  // End token loop

	// Free the last token
	token_free(token);

	return ret_code;
}
