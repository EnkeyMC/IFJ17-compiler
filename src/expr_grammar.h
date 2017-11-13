#ifndef IFJ17_COMPILER_PREC_GRAMMAR_H
#define IFJ17_COMPILER_PREC_GRAMMAR_H

#include <stdbool.h>
#include "sem_analyzer.h"
#include "token.h"

#define NUM_OF_EXPR_RULES 27

/**
 * Enum determining index of a given token in the Precedence Table
 */
typedef enum {
// ARITHMETIC OPERATORS
	PT_INDEX_ADD,
	PT_INDEX_SUB,
	PT_INDEX_UNARY_MINUS,
	PT_INDEX_MUL,
	PT_INDEX_DIVI,
	PT_INDEX_DIVR,
// BOOLEAN OPERATORS
	PT_INDEX_EQUAL,
	PT_INDEX_NE,
	PT_INDEX_GT,
	PT_INDEX_GE,
	PT_INDEX_LT,
	PT_INDEX_LE,
// LITERAL or IDENETIFIER
	PT_INDEX_CONST,
	PT_INDEX_ID,
// BITWISE OPERATOS
	PT_INDEX_NOT,
	PT_INDEX_AND,
	PT_INDEX_OR,
// SPECIAL CHARS
	PT_INDEX_LPAR,
	PT_INDEX_RPAR,
	PT_INDEX_COMMA,
	PT_INDEX_END_MARKER,

// ENUM SIZE
	PT_INDEX_ENUM_SIZE	// Number of rows/columns in the Precedence Table

} pt_index_e;

typedef enum {
    // Only these two can be placed on stack during expression parsing
    EXPR_END_MARKER = TOKEN_ENUM_END, // '$'
	EXPR_HANDLE_MARKER,	// '<'

	EXPR_PUSH_MARKER,	// '='
	EXPR_REDUCE_MARKER,	// '>'
	EXPR_ERROR,	// correponds to blank space in precedence table
	EXPR_SUCCESS
} pt_item_e;

/**
 * Global operator-precedence grammar. Needs to be initialized
 */
extern struct expr_grammar_t {
	Rule* rules[NUM_OF_EXPR_RULES];
	unsigned** precedence_table;	/// 2D array representing Precedence Table
} expr_grammar;

/**
 * Initialize operator-precedence grammar
 */
bool expr_grammar_init();

/**
 * Free expression grammar
 */
void expr_grammar_free();

/**
 * Map token to row/column in Precedence Table
 * @param token token to map index to
 * @return index of column/row
 */
unsigned pt_map_token(unsigned token);

#endif //IFJ17_COMPILER_PREC_GRAMMAR_H
