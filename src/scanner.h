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
	Token* backlog_token; /// Backlog token
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

/**
 * Return backlog token, only one token can be in backlog
 * @param scanner Scanner
 * @param token Token to backlog
 */
void scanner_unget_token(Scanner* scanner, Token* token);

/**
 * Convert white chars to escape sequences
 *  @param string to convert
 *  @return string with escape sequences
 */

char* convert_white_char(const char* str);

#endif //IFJ17_COMPILER_SCANNER_H
