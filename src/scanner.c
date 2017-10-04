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

static FILE* _stream = NULL;

void set_input_stream(FILE* stream) {
	_stream = stream;
}

token_t* get_token() {
	assert(_stream != NULL);

	return NULL;
}

