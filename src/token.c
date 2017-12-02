/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include "token.h"
#include "error_code.h"
#include "debug.h"
#include "memory_manager.h"

Token* token_init() {
	return (Token*) mm_malloc(sizeof(Token));
}

void token_free(Token* token) {
	if (token == NULL)
		return;

	if (token->id == TOKEN_IDENTIFIER || token->id == TOKEN_STRING)
		if (token->data.str != NULL)
			mm_free(token->data.str);
	mm_free(token);
}

unsigned int get_token_column_value(token_e token) {
	return token - TERMINALS_START;
}

Token* token_copy(Token* token) {
	if (token == NULL)
		return NULL;

	Token* copy = token_init();
	copy->id = token->id;

	switch (token->id) {
		case TOKEN_STRING:
		case TOKEN_IDENTIFIER:
			copy->data.str = (char*) mm_malloc(sizeof(char) * (strlen(token->data.str) + 1));
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
	char* copy = (char*) mm_malloc(sizeof(char) * (strlen(string) + 1));

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

	debugs("}");
}
