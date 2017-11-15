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

#include "parser.h"
#include "error_code.h"
#include "expr_grammar.h"
#include "expr_parser.h"


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

	parser->sym_tab_stack = stack_init(1);
	if (parser->sym_tab_stack == NULL) {
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}

	parser->sym_tab_global = htab_init(10);
	if (parser->sym_tab_global == NULL) {
		stack_free(parser->sym_tab_stack, NULL);
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}

	parser->sym_tab_functions = htab_init(10);
	if (parser->sym_tab_functions == NULL) {
		htab_free(parser->sym_tab_global);
		stack_free(parser->sym_tab_stack, NULL);
		stack_free(parser->dtree_stack, NULL);
		free(parser);
		grammar_free();
		return NULL;
	}

	parser->sem_an_stack = stack_init(5);
	if (parser->sem_an_stack == NULL) {
		htab_free(parser->sym_tab_functions);
		htab_free(parser->sym_tab_global);
		stack_free(parser->sym_tab_stack, NULL);
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
	stack_free(parser->sym_tab_stack, NULL);
	stack_free(parser->sem_an_stack, sem_an_free);
	htab_free(parser->sym_tab_functions);
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
	SemAnalyzer* sem_an;

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
							stack_push(parser->sem_an_stack, sem_an);
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

				if (!stack_empty(parser->sem_an_stack)) {
					// Handle semantics
					sem_an = (SemAnalyzer*) stack_top(parser->sem_an_stack);
					sem_an->sem_action(sem_an, parser, token);
					token = NULL;  // Semantic action will take care of freeing token

					Token* symbol = NULL;
					// Finish up semantic analyzers
					while (sem_an != NULL && sem_an->finished) {
						// If semantic action is finished, pop it from stack
						stack_pop(parser->sem_an_stack);
						symbol = sem_an->symbol;  // Store possible symbol for parent semantic analyzer
						sem_an_free(sem_an);  // Free finished semantic analyzer

						// Get parent semantic action
						sem_an = (SemAnalyzer*) stack_top(parser->sem_an_stack);
						if (sem_an != NULL) {
							// Call parent semantic action with symbol from child
							sem_an->sem_action(sem_an, parser, symbol);
						} else {
							token_free(symbol);  // Just in case, free the symbol
						}
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
