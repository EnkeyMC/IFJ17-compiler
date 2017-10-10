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

#include "scanner.h"

void scanner_init(Scanner* scanner) {
	assert(scanner != NULL);
	scanner->stream = stdin;
}

token_t* scanner_get_token(Scanner* scanner) {
	assert(scanner != NULL);
	assert(scanner->stream != NULL);

	return NULL;
}

