/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_SCANNER_H
#define IFJ17_COMPILER_SCANNER_H

#include <stdio.h>
#include "symtable.h"
#include "buffer.h"
#include "token.h"


/**
 * Scanner object structure
 */
typedef struct {
	FILE* stream;  /// Input stream
    Buffer* buffer;  /// Buffer for string and identifier strings
} Scanner;

/**
 * Initialize scanner object
 * @return scanner, NULL on allocation error
 */
Scanner* scanner_init();

/**
 * Free scanner object
 * @param scanner
 */
void scanner_free(Scanner* scanner);

/**
 * Get next token
 * @return token, NULL on allocation error
 */
Token* scanner_get_token(Scanner* scanner);

#endif //IFJ17_COMPILER_SCANNER_H
