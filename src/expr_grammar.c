/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include "expr_grammar.h"
#include "memory_manager.h"

/// Add rule to grammar, cleanup on failure
#define ADD_EXPR_RULE(nt, sem_an, ...) expr_grammar_add_rule(curr_idx++, nt, sem_an, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)


struct expr_grammar_t expr_grammar;


static void expr_grammar_add_rule(int idx, non_terminal_e nt, semantic_action_f sem_an, int va_num, ...) {
	assert(idx < NUM_OF_EXPR_RULES);

	Rule* rule = (Rule*) mm_malloc(sizeof(Rule));

	rule->production = (unsigned*) mm_malloc(sizeof(unsigned) * (va_num + 1));

	va_list va_args;
	va_start(va_args, va_num);
	int i;
	for (i = 0; i < va_num; i++) {
		rule->production[i] = va_arg(va_args, unsigned);
	}

	rule->production[i] = END_OF_RULE;
	rule->for_nt = nt;
	rule->sem_action = sem_an;

	va_end(va_args);

	expr_grammar.rules[idx] = rule;
}

void expr_grammar_init() {
	// Precedence table allocation
	expr_grammar.precedence_table = (unsigned**) mm_malloc(sizeof(unsigned*) * PT_INDEX_ENUM_SIZE);
	for (int j = 0; j < PT_INDEX_ENUM_SIZE; j++) {
		expr_grammar.precedence_table[j] = (unsigned *) mm_malloc(sizeof(unsigned) * PT_INDEX_ENUM_SIZE);
	}

	// Grammar rules init -- Rules already REVERSED !!!
	int curr_idx = 0;	// Starting at index 0
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_func, TOKEN_RPAR, TOKEN_LPAR, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_func, TOKEN_RPAR, NT_EXPRESSION, TOKEN_LPAR, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_func, TOKEN_RPAR, NT_LIST, TOKEN_LPAR, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_LIST, sem_expr_list_expr, NT_EXPRESSION, TOKEN_COMMA, NT_LIST);
	ADD_EXPR_RULE(NT_LIST, sem_expr_list, NT_EXPRESSION, TOKEN_COMMA, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_id, TOKEN_IDENTIFIER);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_const, TOKEN_STRING);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_const, TOKEN_INT);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_const, TOKEN_REAL);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_const, TOKEN_KW_TRUE);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_const, TOKEN_KW_FALSE);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_brackets, TOKEN_RPAR, NT_EXPRESSION, TOKEN_LPAR);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_unary, NT_EXPRESSION, TOKEN_UNARY_MINUS);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_aritmetic_basic, NT_EXPRESSION, TOKEN_ADD, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_aritmetic_basic, NT_EXPRESSION, TOKEN_SUB, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_aritmetic_basic, NT_EXPRESSION, TOKEN_MUL, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_div, NT_EXPRESSION, TOKEN_DIVI, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_div, NT_EXPRESSION, TOKEN_DIVR, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_eq_ne, NT_EXPRESSION, TOKEN_EQUAL, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_eq_ne, NT_EXPRESSION, TOKEN_NE, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_lte_gte, NT_EXPRESSION, TOKEN_GT, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_lte_gte, NT_EXPRESSION, TOKEN_GE, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_lte_gte, NT_EXPRESSION, TOKEN_LT, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_lte_gte, NT_EXPRESSION, TOKEN_LE, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_and_or_not, NT_EXPRESSION, TOKEN_KW_NOT);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_and_or_not, NT_EXPRESSION, TOKEN_KW_AND, NT_EXPRESSION);
	ADD_EXPR_RULE(NT_EXPRESSION, sem_expr_and_or_not, NT_EXPRESSION, TOKEN_KW_OR, NT_EXPRESSION);

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
		"<<<<<<<<<<<<<<<<<<=< ",	// TOKEN_LPAR
		">>>>>>>>>>>>  >>> >>>",	// TOKEN_RPAR
		"<<<<<<<<<<<<<<<<<<>> ",	// TOKEN_COMMA
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
}

void expr_grammar_free() {
	for (int i = 0; i < NUM_OF_EXPR_RULES; i++)
		rule_free(expr_grammar.rules[i]);

	for (int i = 0; i < PT_INDEX_ENUM_SIZE; i++)
		mm_free(expr_grammar.precedence_table[i]);
	mm_free(expr_grammar.precedence_table);
}

unsigned pt_map_token(unsigned token) {
	switch (token) {
		// Arithmetic operators
		case TOKEN_DIVR: return PT_INDEX_DIVR;
		case TOKEN_DIVI: return PT_INDEX_DIVI;
		case TOKEN_MUL: return PT_INDEX_MUL;
		case TOKEN_ADD: return PT_INDEX_ADD;
		case TOKEN_SUB: return PT_INDEX_SUB;
		case TOKEN_UNARY_MINUS: return PT_INDEX_UNARY_MINUS;

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

		// Boolean operators
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
