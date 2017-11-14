#include <malloc.h>
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
