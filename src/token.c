#include <assert.h>
#include <malloc.h>
#include "token.h"

void token_free(Token* token) {
	if (token == NULL)
		return;

	if ((token->id == TOKEN_IDENTIFIER || token->id == TOKEN_STRING) && token->str != NULL)
		free(token->str);
	free(token);
}
