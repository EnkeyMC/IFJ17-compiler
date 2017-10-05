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
    TOKEN_KEYWORD,
    TOKEN_STRING,
    TOKEN_INT,
    TOKEN_REAL,

// EXPANSIONS
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
    token_e id;     /// Type of token
    void* attr; /// Token data attribute
} token_t;

/**
 * Set the input stream  for get_token function. Only used for testing.
 * @param stream Opened file stream
 */
void set_input_stream(FILE* stream);

/**
 * Get next token
 * @return token
 */
token_t* get_token();

#endif //IFJ17_COMPILER_SCANNER_H
