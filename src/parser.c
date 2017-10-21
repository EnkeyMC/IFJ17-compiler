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
	scanner_set_symtable(scanner, symtab_global);

	token_t* token = NULL;

	do {
		if (token != NULL)
			free(token);
		token = scanner_get_token(scanner);
		if (token->id == LEX_ERROR)
			ret_code = EXIT_LEX_ERROR;
	} while (token->id != TOKEN_EOF && token->id != LEX_ERROR);

	scanner_free(scanner);
	htab_free(symtab_global);

	return ret_code;
}
