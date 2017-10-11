/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <ctype.h>

#include "scanner.h"
#include "fsm.h"

#define READ_CHAR() getc(scanner->stream);

void scanner_init(Scanner* scanner) {
	assert(scanner != NULL);
	scanner->stream = stdin;
}

token_t* scanner_get_token(Scanner* scanner) {
	assert(scanner != NULL);
	assert(scanner->stream != NULL);

	int ch;
	token_t* token = (token_t*) malloc(sizeof(token_t));
	token->attr = NULL;

	FSM {
		STATE(s) {
			ch = READ_CHAR();

			if (ch != '\n' && isspace(ch)) {
				NEXT_STATE(s);
			}

			if (ch == '\n') {
				NEXT_STATE(EOL);
			}

			if (ch == ';') {
				NEXT_STATE(semicolon);
			}

			token->id = TOKEN_EOF;
			return token;
		}

		STATE(semicolon) {
			// No need to read character, this is an end state
			token->id = TOKEN_SEMICOLON;
			return token;
		}

		STATE(EOL) {
			token->id = TOKEN_EOL;
			return token;
		}
	}

	free(token);
	return NULL;
}

