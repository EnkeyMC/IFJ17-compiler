/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_TOKEN_H
#define IFJ17_COMPILER_TOKEN_H

#include "grammar.h"

#define IS_TOKEN(expr) ((expr) >= TERMINALS_START && (expr) < END_OF_TERMINALS)

typedef enum {
// ARITMETIC OPERATORS
    TOKEN_EQUAL = TERMINALS_START,
    TOKEN_DIVR,
    TOKEN_DIVI,
    TOKEN_MUL,
    TOKEN_ADD,
    TOKEN_SUB,
	TOKEN_UNARY_MINUS,

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
    TOKEN_KW_DECLARE,
    TOKEN_KW_DIM,
    TOKEN_KW_DO,
    TOKEN_KW_DOUBLE,
    TOKEN_KW_ELSE,
    TOKEN_KW_END,
    TOKEN_KW_FUNCTION,
    TOKEN_KW_IF,
    TOKEN_KW_INPUT,
    TOKEN_KW_INTEGER,
    TOKEN_KW_LOOP,
    TOKEN_KW_PRINT,
    TOKEN_KW_RETURN,
    TOKEN_KW_SCOPE,
    TOKEN_KW_STRING,
    TOKEN_KW_THEN,
    TOKEN_KW_WHILE,
    TOKEN_KW_UNTIL,

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
    TOKEN_KW_TO,
    TOKEN_KW_STEP,

    TOKEN_SUB_ASIGN,
    TOKEN_DIVR_ASIGN,
    TOKEN_DIVI_ASIGN,
    TOKEN_MUL_ASIGN,
    TOKEN_ADD_ASIGN,

// ENDING TOKENS
    TOKEN_EOF,

    END_OF_TERMINALS,  // Not actual token
    LEX_ERROR,

    TOKEN_ENUM_END  // Mark starting point for precedence table items
} token_e;

/**
 * Union for token data
 */
union token_data {
    int i;
    double d;
    char* str;
};

/**
 * Token structure type
 */
typedef struct token_t {
    token_e id;		/// Type of token
    union token_data data;  /// Token data
} Token;

/**
 * Allocate new token
 * @return new Token
 */
Token* token_init();

/**
 * Free token
 * @param token
 */
void token_free(Token* token);

/**
 * Get column index to LL table from token
 * @param token
 * @return column index to LL table
 */
unsigned int get_token_column_value(token_e token);

/**
 * Deep copy a token
 * @param token Token to copy
 * @return copied token
 */
Token* token_copy(Token* token);

/**
 * Make new token with given type and data
 * @param type Type of token
 * @param data Token data
 * @return Token
 */
Token token_make(token_e type, union token_data data);

/**
 * Make new token with string type
 * @param string String is copied to token
 * @return Token
 */
Token token_make_str(const char* string);

/**
 * Print debug info about token
 * @param t Token
 */
void token_debug(void* t);

#endif //IFJ17_COMPILER_TOKEN_H
