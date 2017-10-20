/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include "scanner.h"

int parse() {
	HashTable* symtab_global = htab_init(0);
	Scanner* scanner = scanner_init();
	scanner_set_symtable(scanner, symtab_global);

	token_t* token;

	do {
		token = scanner_get_token(scanner);
	} while (token->id != TOKEN_EOF);

	free(scanner);
	htab_free(symtab_global);

	return 0;
}
