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

// Token ID definitions

// ARITMETIC OPERATORS
#define TOKEN_DIVR          10
#define TOKEN_DIVI          11
#define TOKEN_MUL           12
#define TOKEN_ADD           13
#define TOKEN_SUB           14
#define TOKEN_EQUAL         15

// BOOLEAN OPERATORS
#define TOKEN_LT            20
#define TOKEN_GT            21
#define TOKEN_LE            22
#define TOKEN_GE            23
#define TOKEN_NE            24

// SPECIAL TOKENS
#define TOKEN_COMMA         30
#define TOKEN_SEMICOLON     31
#define TOKEN_LPAR          32
#define TOKEN_RPAR          33

// STRING TOKENS
#define TOKEN_IDENTIFIER    40
#define TOKEN_KEYWORD       41
#define TOKEN_STRING        42
#define TOKEN_INT           43
#define TOKEN_REAL          44

// EXPANSIONS
#define TOKEN_DIVR_ASIGN    50
#define TOKEN_DIVI_ASIGN    51
#define TOKEN_MUL_ASIGN     52
#define TOKEN_ADD_ASIGN     53
#define TOKEN_SUB_ASIGN     54


/**
 * Token structure type
 */
typedef struct {
    int id;     /// Token ID (see Token ID constants)
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
