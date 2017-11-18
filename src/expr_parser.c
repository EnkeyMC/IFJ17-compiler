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

		// look for next action in the precedence table
		unsigned action = GET_ACTION(s, token->id);
		while (action == EXPR_REDUCE_MARKER) {
			ret_code = ext_stack_reduce(s, parser);
			if (ret_code == EXIT_SUCCESS)
				action = GET_ACTION(s, token->id);
			else
				break;
		}

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
		else
			break;
	} while (ret_code == EXIT_SUCCESS);

	// Return token that is not part of expression
	scanner_unget_token(parser->scanner, token);

	ext_stack_free(s);

	return ret_code;
}
