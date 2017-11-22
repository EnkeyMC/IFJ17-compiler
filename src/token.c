#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include "token.h"
#include "error_code.h"
#include "debug.h"

void token_free(Token* token) {
	if (token == NULL)
		return;

	if (token->id == TOKEN_IDENTIFIER || token->id == TOKEN_STRING)
		if (token->data.str != NULL)
			free(token->data.str);
	free(token);
}

unsigned int get_token_column_value(token_e token) {
	return token - TERMINALS_START;
}

Token* token_copy(Token* token) {
	if (token == NULL)
		return NULL;

	Token* copy = (Token*) malloc(sizeof(Token));
	if (copy == NULL) {
		return NULL;
	}

	copy->id = token->id;

	switch (token->id) {
		case TOKEN_STRING:
		case TOKEN_IDENTIFIER:
			copy->data.str = (char*) malloc(sizeof(char) * (strlen(token->data.str) + 1));
			if (copy->data.str == NULL) {
				free(copy);
				return NULL;
			}
			strcpy(copy->data.str, token->data.str);
			break;
		case TOKEN_INT:
			copy->data.i = token->data.i;
			break;
		case TOKEN_REAL:
			copy->data.d = token->data.d;
			break;
		default:
			copy->data.str = NULL;
	}

	return copy;
}

Token token_make(token_e type, union token_data data) {
	Token token;
	token.id = type;
	token.data = data;
	return token;
}

Token token_make_str(const char* string) {
	Token token;
	token.id = TOKEN_STRING;
	char* copy = (char*) malloc(sizeof(char) * (strlen(string) + 1));
	if (copy == NULL) exit(EXIT_INTERN_ERROR);  // I do not have time to make proper exit, sorry

	strcpy(copy, string);

	token.data.str = copy;
	return token;
}

void token_debug(void* t) {
	Token* token = (Token*) t;

	debug("Token@%p: {", token);

	if (token != NULL) {
		debug(".id = %d", token->id);

		switch (token->id) {
			case TOKEN_STRING:
			case TOKEN_IDENTIFIER:
				debug(", .data = %s", token->data.str);
				break;
			case TOKEN_INT:
				debug(", .data = %d", token->data.i);
				break;
			case TOKEN_REAL:
				debug(", .data = %g", token->data.d);
				break;
			default:
				break;
		}
	}

	debug("}");
}
