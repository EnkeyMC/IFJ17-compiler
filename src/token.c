#include <malloc.h>
#include <memory.h>
#include "token.h"

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
