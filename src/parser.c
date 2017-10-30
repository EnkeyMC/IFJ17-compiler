/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <stdlib.h>
#include "scanner.h"
#include "error_code.h"

int parse() {
	int ret_code = EXIT_SUCCESS;

	HashTable* symtab_global = htab_init(0);
	if (symtab_global == NULL)
		return EXIT_INTERN_ERROR;

	Scanner* scanner = scanner_init();
	if (scanner == NULL) {
		htab_free(symtab_global);
		return EXIT_INTERN_ERROR;
	}

	if (!grammar_init()) {
		htab_free(symtab_global);
		scanner_free(scanner);
		return EXIT_INTERN_ERROR;
	}

	Token* token = NULL;

	do {
		token_free(token);

		token = scanner_get_token(scanner);
		if (token == NULL) {
			ret_code = EXIT_INTERN_ERROR;
			break;
		} else if (token->id == LEX_ERROR) {
			ret_code = EXIT_LEX_ERROR;
			break;
		}
	} while (token->id != TOKEN_EOF);

	token_free(token);

	scanner_free(scanner);
	htab_free(symtab_global);
	grammar_free();

	return ret_code;
}
