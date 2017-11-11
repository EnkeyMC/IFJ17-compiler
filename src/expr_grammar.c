#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include "expr_grammar.h"
#include "grammar.h"

/// Add rule to grammar, cleanup on failure
#define ADD_EXPR_RULE(nt, ...) if (!expr_grammar_add_rule(curr_idx++, nt, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)) { expr_grammar_free(); return false; }


struct expr_grammar_t expr_grammar;


static bool expr_grammar_add_rule(int idx, non_terminal_e nt, int va_num, ...) {
	assert(idx < NUM_OF_EXPR_RULES);

	Rule* rule = (Rule*) malloc(sizeof(Rule));
	if (rule == NULL) {
		return false;
	}

	rule->production = (unsigned*) malloc(sizeof(unsigned) * va_num + 1);
	if (rule->production == NULL) {
		free(rule);
		return false;
	}

	va_list va_args;
	va_start(va_args, va_num);
	int i;
	for (i = 0; i < va_num; i++) {
		rule->production[i] = va_arg(va_args, unsigned);
	}

	rule->production[i] = END_OF_RULE;
	rule->for_nt = nt;

	va_end(va_args);

	expr_grammar.rules[idx] = rule;

	return true;
}

bool expr_grammar_init() {
	// Precedence table allocation
	expr_grammar.precedence_table = malloc(sizeof(unsigned) * PT_INDEX_ENUM_SIZE * PT_INDEX_ENUM_SIZE);
	if (expr_grammar.precedence_table == NULL)
		return false;

	// Grammar rules init -- Rules already REVERSED !!!
	int curr_idx = 0;	// Starting at index 0
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_RPAR, TOKEN_LPAR, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_RPAR, NT_EXPRESSION, TOKEN_LPAR, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_RPAR, NT_LIST, TOKEN_LPAR, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_LIST, NT_EXPRESSION, TOKEN_COMMA, NT_LIST);
	ADD_EXPR_RULE(NT_LIST, NT_EXPRESSION, TOKEN_COMMA, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_STRING);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_INT);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_REAL);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_KW_TRUE);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_KW_FALSE);
	ADD_EXPR_RULE(NT_EXPRESSION, TOKEN_RPAR, NT_EXPRESSION, TOKEN_LPAR);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_SUB);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_ADD, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_SUB, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_MUL, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_DIVI, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_DIVR, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_EQUAL, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_NE, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_GT, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_GE, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_LT, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_LE, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_KW_NOT);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_KW_AND, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, NT_EXPRESSION, TOKEN_KW_OR, NT_EXPRESSION);

	// Precedence table init
	const char *rows[] = {
		">><<<<>>>>>><<<>><>>>",	// TOKEN_ADD
		">><<<<>>>>>><<<>><>>>",	// TOKEN_SUB
		">><>>>>>>>>><<>>><>>>",	// UNARY_MINUS
		">><>>>>>>>>><<<>><>>>",	// TOKEN_MUL
		">><<><>>>>>><<<>><>>>",	// TOKEN_DIVI
		">><>>>>>>>>><<<>><>>>",	// TOKEN_DIVR
		"<<<<<<      <<<>><>>>",	// TOKEN_EQUAL
		"<<<<<<      <<<>><>>>",	// TOKEN_NE
		"<<<<<<      <<<>><>>>",	// TOKEN_GT
		"<<<<<<      <<<>><>>>",	// TOKEN_GE
		"<<<<<<      <<<>><>>>",	// TOKEN_LT
		"<<<<<<      <<<>><>>>",	// TOKEN_LE
		">> >>>>>>>>>   >> >>>",	// CONST
		">> >>>>>>>>>   >>=>>>",	// TOKEN_IDENTIFIER
		"<<<<<<<<<<<<<<<>><>>>",	// TOKEN_KW_NOT
		"<<<<<<<<<<<<<<<>><>>>",	// TOKEN_KW_AND
		"<<<<<<<<<<<<<<<<><>>>",	// TOKEN_KW_OR
		"<<<<<<<<<<<<<<<<<<== ",	// TOKEN_LPAR
		">>>>>>>>>>>>  >>> >>>",	// TOKEN_RPAR
		"<<<<<<<<<<<<<<<<<<== ",	// TOKEN_COMMA
		"<<<<<<<<<<<<<<<<<<  $"		// END_MARKER
	};

	for (unsigned i = 0; i < PT_INDEX_ENUM_SIZE; i++)
		for (unsigned k = 0; k < PT_INDEX_ENUM_SIZE; k++) {
			if (rows[i][k] == '<')
				expr_grammar.precedence_table[i][k] = EXPR_HANDLE_MARKER;
			else if (rows[i][k] == '>')
				expr_grammar.precedence_table[i][k] = EXPR_REDUCE_MARKER;
			else if (rows[i][k] == '=')
				expr_grammar.precedence_table[i][k] = EXPR_PUSH_MARKER;
			else if (rows[i][k] == ' ')
				expr_grammar.precedence_table[i][k] = EXPR_ERROR;
			else
				expr_grammar.precedence_table[i][k] = EXPR_SUCCESS;
		}

	return true;
}

void expr_grammar_free() {
	for (int i = 0; i < NUM_OF_EXPR_RULES; i++)
		rule_free(expr_grammar.rules[i]);

	free(expr_grammar.precedence_table);
}

unsigned pt_map_token(token_e token) {
	switch (token) {
		// Arithmetic operators
		case TOKEN_DIVR: return PT_INDEX_DIVR;
		case TOKEN_DIVI: return PT_INDEX_DIVI;
		case TOKEN_MUL: return PT_INDEX_MUL;
		case TOKEN_ADD: return PT_INDEX_ADD;
		case TOKEN_SUB: return PT_INDEX_SUB;
			// UNARY_MINUS NOT HANDLED YET

		// Boolean operators
		case TOKEN_EQUAL: return PT_INDEX_EQUAL;
		case TOKEN_LT: return PT_INDEX_LT;
		case TOKEN_GT: return PT_INDEX_GT;
		case TOKEN_LE: return PT_INDEX_LE;
		case TOKEN_GE: return PT_INDEX_GE;
		case TOKEN_NE: return PT_INDEX_NE;

		// Literal or identifier
		case TOKEN_INT: return PT_INDEX_CONST;
		case TOKEN_STRING: return PT_INDEX_CONST;
		case TOKEN_REAL: return PT_INDEX_CONST;
		case TOKEN_KW_FALSE: return PT_INDEX_CONST;
		case TOKEN_KW_TRUE: return PT_INDEX_CONST;
		case TOKEN_IDENTIFIER: return PT_INDEX_ID;

		// Bitwise operators
		case TOKEN_KW_NOT: return PT_INDEX_NOT;
		case TOKEN_KW_AND: return PT_INDEX_AND;
		case TOKEN_KW_OR: return PT_INDEX_OR;

		// Special chars
		case TOKEN_LPAR: return PT_INDEX_LPAR;
		case TOKEN_RPAR: return PT_INDEX_RPAR;
		case TOKEN_COMMA: return PT_INDEX_COMMA;

		default: return PT_INDEX_END_MARKER;
	}
}
