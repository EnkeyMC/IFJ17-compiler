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


typedef enum {
// ARITMETIC OPERATORS
	TOKEN_EQUAL,
	TOKEN_DIVR,
	TOKEN_DIVI,
	TOKEN_MUL,
	TOKEN_ADD,
	TOKEN_SUB,

// BOOLEAN OPERATORS
	TOKEN_LT,
	TOKEN_GT,
	TOKEN_LE,
	TOKEN_GE,
	TOKEN_NE,

// SPECIAL TOKENS
	TOKEN_COMMA,
	TOKEN_SEMICOLON,
	TOKEN_LPAR,
	TOKEN_RPAR,
	TOKEN_EOL,

// STRING TOKENS
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_INT,
	TOKEN_REAL,

// KEYWORDS
	TOKEN_KW_AS,
	TOKEN_KW_ASC,
	TOKEN_KW_DECLARE,
	TOKEN_KW_DIM,
	TOKEN_KW_DO,
	TOKEN_KW_DOUBLE,
	TOKEN_KW_ELSE,
	TOKEN_KW_END,
	TOKEN_KW_CHR,
	TOKEN_KW_FUNCTION,
	TOKEN_KW_IF,
	TOKEN_KW_INPUT,
	TOKEN_KW_INTEGER,
	TOKEN_KW_LENGTH,
	TOKEN_KW_LOOP,
	TOKEN_KW_PRINT,
	TOKEN_KW_RETURN,
	TOKEN_KW_SCOPE,
	TOKEN_KW_STRING,
	TOKEN_KW_SUBSTR,
	TOKEN_KW_THEN,
	TOKEN_KW_WHILE,

// EXPANSIONS

// RESERVED KEYWORDS
	TOKEN_KW_AND,
	TOKEN_KW_BOOLEAN,
	TOKEN_KW_CONTINUE,
	TOKEN_KW_ELSEIF,
	TOKEN_KW_EXIT,
	TOKEN_KW_FALSE,
	TOKEN_KW_FOR,
	TOKEN_KW_NEXT,
	TOKEN_KW_NOT,
	TOKEN_KW_OR,
	TOKEN_KW_SHARED,
	TOKEN_KW_STATIC,
	TOKEN_KW_TRUE,

	TOKEN_SUB_ASIGN,
	TOKEN_DIVR_ASIGN,
	TOKEN_DIVI_ASIGN,
	TOKEN_MUL_ASIGN,
	TOKEN_ADD_ASIGN,

// ENDING TOKENS
	TOKEN_EOF,
	LEX_ERROR
} token_e;


/**
 * Token structure type
 */
typedef struct {
	token_e id;		/// Type of token
	void* attr;		/// Token data attribute
} token_t;

/**
 * Scanner object structure
 */
typedef struct {
	FILE* stream;  /// Input stream
	// Current scope and maybe other stuff
} Scanner;

/**
 * Initialize scanner object
 * @param scanner
 */
void scanner_init(Scanner* scanner);

/**
 * Get next token
 * @return token
 */
token_t* scanner_get_token(Scanner* scanner);

#endif //IFJ17_COMPILER_SCANNER_H
