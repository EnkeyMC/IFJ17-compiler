/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <assert.h>
#include <stdlib.h>

#include "expr_grammar.h"
#include "expr_parser.h"
#include "ext_stack.h"
#include "error_code.h"

#define GET_ACTION(s, token) (expr_grammar.precedence_table[pt_map_token(ext_stack_top(s))][pt_map_token(token)])

int parse_expression(Parser *parser) {
	assert(parser != NULL);

	ExtStack* s = ext_stack_init();
	if (s == NULL)
		return EXIT_INTERN_ERROR;

	int ret_code = EXIT_SUCCESS;
	Token* token = NULL;
	token_e last_token = END_OF_TERMINALS;

	do {
		token_free(token);

		token = scanner_get_token(parser->scanner);
		if (token == NULL) {
			ret_code = EXIT_INTERN_ERROR;
			break;
		}
		else if (token->id == LEX_ERROR) {
			ret_code = EXIT_LEX_ERROR;
			break;
		}

		// Handle unary minus
		if (token->id == TOKEN_SUB) {
			switch (last_token) {
				case END_OF_TERMINALS:
				case TOKEN_ADD:
				case TOKEN_SUB:
				case TOKEN_DIVI:
				case TOKEN_DIVR:
				case TOKEN_MUL:
				case TOKEN_UNARY_MINUS:
				case TOKEN_EQUAL:
				case TOKEN_LT:
				case TOKEN_GT:
				case TOKEN_LE:
				case TOKEN_GE:
				case TOKEN_NE:
				case TOKEN_LPAR:
				case TOKEN_COMMA:
					token->id = TOKEN_UNARY_MINUS;
				default: break;
			}
		}

		// look for next action in the precedence table
		unsigned action = GET_ACTION(s, token->id);
		while (action == EXPR_REDUCE_MARKER) {
			ret_code = ext_stack_reduce(s, parser);
			if (ret_code == EXIT_SUCCESS)
				action = GET_ACTION(s, token->id);
			else
				break;
		}

		last_token = token->id;	// update last token

		if (action == EXPR_PUSH_MARKER) {
			if (! ext_stack_push(s, token->id, token)) {
				ret_code = EXIT_INTERN_ERROR;
			}
		}
		else if (action == EXPR_HANDLE_MARKER) {
			if (! ext_stack_shift(s, token)) {
				ret_code = EXIT_INTERN_ERROR;
			}
		}
		else if (action == EXPR_ERROR) {
			ret_code = EXIT_SYNTAX_ERROR;
		}
		else {
			if (ret_code == EXIT_SUCCESS)
				if (!ext_stack_expr_on_top(s))
					ret_code = EXIT_SYNTAX_ERROR;
			break;
		}
	} while (ret_code == EXIT_SUCCESS);

	if (ret_code == EXIT_SUCCESS) {
		// After expression evaluation prepare value for parent SemAnalyzer
		SemAnalyzer* sem_an = (SemAnalyzer*) sem_stack_top(parser->sem_an_stack);

		if (sem_an != NULL) {
			// Reuse the SemAnalyzer and call sem_expr_result
			sem_an->sem_action = sem_expr_result;
			sem_an->state = SEM_STATE_START;
			sem_an->finished = false;
			assert(sem_an->value != NULL);
			sem_an->sem_action(sem_an, parser, *sem_an->value);
		}
	}

	// Return token that is not part of expression
	scanner_unget_token(parser->scanner, token);

	ext_stack_free(s);

	return ret_code;
}
