#include <malloc.h>
#include <memory.h>
#include "token.h"

void token_free(Token* token) {
	if (token == NULL)
		return;

	if (token->id == TOKEN_IDENTIFIER || token->id == TOKEN_STRING)
		if (token->str != NULL)
			free(token->str);
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
			copy->str = (char*) malloc(sizeof(char) * (strlen(token->str) + 1));
			if (copy->str == NULL) {
				free(copy);
				return NULL;
			}
			strcpy(copy->str, token->str);
			break;
		case TOKEN_INT:
			copy->i = token->i;
			break;
		case TOKEN_REAL:
			copy->d = token->d;
			break;
		default:
			copy->str = NULL;
	}

	return copy;
}
