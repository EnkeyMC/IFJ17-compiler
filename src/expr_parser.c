#include <assert.h>
#include <stdlib.h>

#include "expr_grammar.h"
#include "expr_parser.h"
#include "ext_stack.h"
#include "error_code.h"

#define get_action(s, token) (expr_grammar.precedence_table[pt_map_token(ext_stack_top(s))][pt_map_token(token)])

int parse_expression(Parser *parser) {
	assert(parser != NULL);

	ExtStack* s = ext_stack_init();
	if (s == NULL)
		return EXIT_INTERN_ERROR;

	int ret_code = EXIT_SUCCESS;
	Token* token = NULL;

	while (1) {
		if (token != NULL) {
			token_free(token);
			parser->scanner->backlog_token = NULL;
		}

		token = scanner_get_token(parser->scanner);
		if (token == NULL) {
			ret_code = EXIT_INTERN_ERROR;
			break;
		}
		else if (token->id == LEX_ERROR) {
			ret_code = EXIT_LEX_ERROR;
			break;
		}

		// save current token to shard buffer
		parser->scanner->backlog_token = token;

		// look for next action in the precedence table
		unsigned action = get_action(s, token->id);
		while (action == EXPR_REDUCE_MARKER) {
			if (ext_stack_reduce(s))
				action = get_action(s, token->id);
			else {
				// haven't found any rule to reduce the stack
				ret_code = EXIT_SYNTAX_ERROR;
				break;
			}
		}

		// syntax error encountered by previous loop
		if (ret_code == EXIT_SYNTAX_ERROR)
			break;
		else if (action == EXPR_PUSH_MARKER) {
			if (! ext_stack_push(s, token->id)) {
				ret_code = EXIT_INTERN_ERROR;
				break;
			}
		}
		else if (action == EXPR_HANDLE_MARKER) {
			if (! ext_stack_shift(s, token->id)) {
				ret_code = EXIT_INTERN_ERROR;
				break;
			}
		}
		else if (action == EXPR_ERROR) {
			ret_code = EXIT_SYNTAX_ERROR;
			break;
		}
		else
			break;
	}

	// Last token is still in the shared buffer

	ext_stack_free(s);

	return ret_code;
}
