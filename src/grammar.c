#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include "grammar.h"
#include "token.h"

/// Get number of variable arguments
#define NUM_ARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

/// Add epsilon rule to grammar, cleanup on failure
#define ADD_EPSILON_RULE(nt)  if (!grammar_add_epsilon_rule(curr_idx++, nt)) { grammar_free(); return false; }
/// Add rule to grammar, cleanup on failure
#define ADD_RULE(nt, ...) if (!grammar_add_rule(curr_idx++, nt, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)) { grammar_free(); return false; }

/// Set table value, cleanup on failure
#define TABLE_SET(row, column, value) if (!sparse_table_set(grammar.LL_table, row, get_token_column_value(column), value)) { grammar_free(); return false; }


struct grammar_t grammar;


static void array_reverse(int* array, int length) {
	int tmp;
	for (int i = 0; i < length / 2; i++) {
		tmp = array[i];
		array[i] = array[length - i - 1];
		array[length - i - 1] = tmp;
	}
}

static bool grammar_add_epsilon_rule(int idx, non_terminal_e nt) {
	assert(idx < NUM_OF_RULES);

	Rule* rule = (Rule*) malloc(sizeof(Rule));
	if (rule == NULL) {
		return false;
	}

	rule->production = (int*) malloc(sizeof(int));
	if (rule->production == NULL) {
		free(rule);
		return false;
	}

	rule->production[0] = END_OF_RULE;
	rule->for_nt = nt;

	grammar.rules[idx] = rule;

	return true;
}

static bool grammar_add_rule(int idx, non_terminal_e nt, int va_num, ...) {
	assert(idx < NUM_OF_RULES);

	Rule* rule = (Rule*) malloc(sizeof(Rule));
	if (rule == NULL) {
		return false;
	}

	rule->production = (int*) malloc(sizeof(int) * va_num + 1);
	if (rule->production == NULL) {
		free(rule);
		return false;
	}

	va_list va_args;
	va_start(va_args, va_num);
	int i;
	for (i = 0; i < va_num; i++) {
		rule->production[i] = va_arg(va_args, int);
	}

	array_reverse(rule->production, va_num);  // We are going to push it on stack in reverse order
	rule->production[i] = END_OF_RULE;
	rule->for_nt = nt;

	va_end(va_args);

	grammar.rules[idx] = rule;

	return true;
}

static void rule_free(Rule* rule) {
	if (rule != NULL) {
		if (rule->production != NULL)
			free(rule->production);
		free(rule);
	}
}

bool grammar_init() {
	// LL table init
	grammar.LL_table = sparse_table_init(NT_ENUM_SIZE, END_OF_TERMINALS - TERMINALS_START, 0);
	if (grammar.LL_table == NULL)
		return false;

	int curr_idx = 0;
	grammar.rules[curr_idx++] = NULL;  // First index needs to be empty

	ADD_RULE(NT_LINE, NT_STATEMENT, NT_LINE_END);
	ADD_RULE(NT_LINE_END, TOKEN_EOL, NT_LINE);
	ADD_EPSILON_RULE(NT_LINE_END);
	ADD_RULE(NT_STATEMENT, NT_INNER_STMT);
	ADD_RULE(NT_STATEMENT, NT_FUNC_DECL);
	ADD_RULE(NT_STATEMENT, NT_FUNC_DEF);
	ADD_RULE(NT_INNER_STMT, NT_VAR_DECL);
	ADD_RULE(NT_INNER_STMT, NT_ASSIGNMENT);
	ADD_RULE(NT_INNER_STMT, NT_IF_STMT);
	ADD_RULE(NT_INNER_STMT, NT_SCOPE_STMT);
	ADD_RULE(NT_INNER_STMT, NT_DO_STMT);
	ADD_RULE(NT_INNER_STMT, NT_FOR_STMT);
	ADD_RULE(NT_INNER_STMT, NT_PRINT_STMT);
	ADD_RULE(NT_INNER_STMT, NT_INPUT_STMT);
	ADD_EPSILON_RULE(NT_INNER_STMT);
	ADD_RULE(NT_STMT_SEQ, NT_INNER_STMT, TOKEN_EOL, NT_STMT_SEQ);
	ADD_EPSILON_RULE(NT_STMT_SEQ);
	ADD_RULE(NT_VAR_DECL, TOKEN_KW_DIM, NT_VAR_DECL_NEXT);
	ADD_RULE(NT_VAR_DECL, TOKEN_KW_STATIC, NT_VAR_DEF);
	ADD_RULE(NT_VAR_DECL_NEXT, NT_VAR_DEF);
	ADD_RULE(NT_VAR_DECL_NEXT, TOKEN_KW_SHARED, NT_VAR_DEF);
	ADD_RULE(NT_VAR_DEF, TOKEN_IDENTIFIER, TOKEN_KW_AS, NT_TYPE, NT_INIT_OPT);
	ADD_RULE(NT_INIT_OPT, TOKEN_EQUAL, NT_EXPRESSION);
	ADD_EPSILON_RULE(NT_INIT_OPT);
	ADD_RULE(NT_FUNC_DECL, TOKEN_KW_DECLARE, TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, NT_PARAMS, TOKEN_RPAR, TOKEN_KW_AS, NT_TYPE);
	ADD_RULE(NT_TYPE, TOKEN_KW_INTEGER);
	ADD_RULE(NT_TYPE, TOKEN_KW_DOUBLE);
	ADD_RULE(NT_TYPE, TOKEN_KW_STRING);
	ADD_RULE(NT_TYPE, TOKEN_KW_BOOLEAN);
	ADD_RULE(NT_FUNC_DEF, TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, NT_PARAMS, TOKEN_RPAR, TOKEN_KW_AS, NT_TYPE, TOKEN_EOL, NT_FUNC_BODY, TOKEN_KW_END, TOKEN_KW_FUNCTION);
	ADD_RULE(NT_FUNC_BODY, NT_STMT_SEQ);
	ADD_RULE(NT_FUNC_BODY, TOKEN_KW_RETURN, NT_EXPRESSION, TOKEN_EOL, NT_FUNC_BODY);
	ADD_RULE(NT_PARAM_DECL, TOKEN_IDENTIFIER, TOKEN_KW_AS, NT_TYPE);
	ADD_RULE(NT_PARAMS, NT_PARAM_DECL, NT_PARAMS_NEXT);
	ADD_EPSILON_RULE(NT_PARAMS);
	ADD_RULE(NT_PARAMS_NEXT, TOKEN_COMMA, NT_PARAM_DECL, NT_PARAMS_NEXT);
	ADD_EPSILON_RULE(NT_PARAMS_NEXT);
	ADD_RULE(NT_ASSIGNMENT, TOKEN_IDENTIFIER, NT_ASSIGN_OPERATOR, NT_EXPRESSION);
	ADD_RULE(NT_INPUT_STMT, TOKEN_KW_INPUT, TOKEN_IDENTIFIER);
	ADD_RULE(NT_PRINT_STMT, TOKEN_KW_PRINT, NT_EXPRESSION, TOKEN_SEMICOLON, NT_EXPRESSION_LIST);
	ADD_RULE(NT_EXPRESSION_LIST, NT_EXPRESSION, TOKEN_SEMICOLON, NT_EXPRESSION_LIST);
	ADD_EPSILON_RULE(NT_EXPRESSION_LIST);
	ADD_RULE(NT_SCOPE_STMT, TOKEN_KW_SCOPE, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_END, TOKEN_KW_SCOPE);
	ADD_RULE(NT_IF_STMT, TOKEN_KW_IF, NT_EXPRESSION, TOKEN_KW_THEN, TOKEN_EOL, NT_STMT_SEQ, NT_IF_STMT_CONT);
	ADD_RULE(NT_IF_STMT_CONT, TOKEN_KW_END, TOKEN_KW_IF);
	ADD_RULE(NT_IF_STMT_CONT, TOKEN_KW_ELSE, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_END, TOKEN_KW_IF);
	ADD_RULE(NT_IF_STMT_CONT, TOKEN_KW_ELSEIF, NT_EXPRESSION, TOKEN_KW_THEN, TOKEN_EOL, NT_STMT_SEQ, NT_IF_STMT_END);
	ADD_RULE(NT_IF_STMT_END, TOKEN_KW_END, TOKEN_KW_IF);
	ADD_RULE(NT_IF_STMT_END, TOKEN_KW_ELSE, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_END, TOKEN_KW_IF);
	ADD_RULE(NT_DO_STMT, TOKEN_KW_DO, NT_DO_STMT_END);
	ADD_RULE(NT_DO_STMT_END, NT_TEST_TYPE, NT_EXPRESSION, TOKEN_EOL, NT_DO_BODY, TOKEN_KW_LOOP);
	ADD_RULE(NT_DO_STMT_END, TOKEN_EOL, NT_DO_BODY, TOKEN_KW_LOOP, NT_TEST_TYPE, NT_EXPRESSION);
	ADD_RULE(NT_TEST_TYPE, TOKEN_KW_WHILE);
	ADD_RULE(NT_TEST_TYPE, TOKEN_KW_UNTIL);
	ADD_RULE(NT_DO_BODY, NT_STMT_SEQ);
	ADD_RULE(NT_DO_BODY, TOKEN_KW_EXIT, TOKEN_KW_DO, TOKEN_EOL, NT_DO_BODY);
	ADD_RULE(NT_DO_BODY, TOKEN_KW_CONTINUE, TOKEN_KW_DO, TOKEN_EOL, NT_DO_BODY);
	ADD_RULE(NT_FOR_STMT, TOKEN_KW_FOR, TOKEN_IDENTIFIER, NT_TYPE_OPT, TOKEN_EQUAL, NT_EXPRESSION, TOKEN_KW_TO, NT_EXPRESSION, NT_STEP_OPT, TOKEN_EOL, NT_FOR_BODY, TOKEN_KW_NEXT, NT_ID_OPT, TOKEN_EOL);
	ADD_RULE(NT_FOR_BODY, NT_STMT_SEQ);
	ADD_RULE(NT_FOR_BODY, TOKEN_KW_EXIT, TOKEN_KW_FOR, TOKEN_EOL, NT_FOR_BODY);
	ADD_RULE(NT_FOR_BODY, TOKEN_KW_CONTINUE, TOKEN_KW_FOR, TOKEN_EOL, NT_FOR_BODY);
	ADD_RULE(NT_TYPE_OPT, TOKEN_KW_AS, NT_TYPE);
	ADD_EPSILON_RULE(NT_TYPE_OPT);
	ADD_RULE(NT_STEP_OPT, TOKEN_KW_STEP, NT_EXPRESSION);
	ADD_EPSILON_RULE(NT_STEP_OPT);
	ADD_RULE(NT_ID_OPT, TOKEN_IDENTIFIER);
	ADD_EPSILON_RULE(NT_ID_OPT);
	ADD_RULE(NT_ASSIGN_OPERATOR, TOKEN_EQUAL);
	ADD_RULE(NT_ASSIGN_OPERATOR, TOKEN_SUB_ASIGN);
	ADD_RULE(NT_ASSIGN_OPERATOR, TOKEN_ADD_ASIGN);
	ADD_RULE(NT_ASSIGN_OPERATOR, TOKEN_MUL_ASIGN);
	ADD_RULE(NT_ASSIGN_OPERATOR, TOKEN_DIVI_ASIGN);
	ADD_RULE(NT_ASSIGN_OPERATOR, TOKEN_DIVR_ASIGN);
	ADD_RULE(NT_EXPRESSION, TOKEN_IDENTIFIER);
	ADD_RULE(NT_EXPRESSION, TOKEN_INT);
	ADD_RULE(NT_EXPRESSION, TOKEN_REAL);
	ADD_RULE(NT_EXPRESSION, TOKEN_STRING);
	ADD_RULE(NT_EXPRESSION, TOKEN_KW_TRUE);
	ADD_RULE(NT_EXPRESSION, TOKEN_KW_FALSE);

	// LL table initialization

	// LINE
	TABLE_SET(NT_LINE, TOKEN_KW_DECLARE, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_DIM, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_DO, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_FUNCTION, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_IF, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_INPUT, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_PRINT, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_SCOPE, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_FOR, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_STATIC, 1);
	TABLE_SET(NT_LINE, TOKEN_IDENTIFIER, 1);
	TABLE_SET(NT_LINE, TOKEN_EOL, 1);
	TABLE_SET(NT_LINE, TOKEN_EOF, 1);
	// LINE_END
	TABLE_SET(NT_LINE_END, TOKEN_EOL, 2);
	TABLE_SET(NT_LINE_END, TOKEN_EOF, 3);
	// STATEMENT
	TABLE_SET(NT_STATEMENT, TOKEN_KW_DECLARE, 5);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_DIM, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_DO, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_FUNCTION, 6);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_IF, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_INPUT, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_PRINT, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_SCOPE, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_FOR, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_STATIC, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_IDENTIFIER, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_EOL, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_EOF, 4);
	// INNER_STMT
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_DIM, 7);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_DO, 11);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_IF, 9);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_INPUT, 14);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_PRINT, 13);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_SCOPE, 10);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_FOR, 12);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_STATIC, 7);
	TABLE_SET(NT_INNER_STMT, TOKEN_IDENTIFIER, 8);
	TABLE_SET(NT_INNER_STMT, TOKEN_EOL, 15);
	TABLE_SET(NT_INNER_STMT, TOKEN_EOF, 15);
	// STMT_SEQ
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_DIM, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_DO, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_ELSE, 17);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_END, 17);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_AND, 17);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_IF, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_INPUT, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_LOOP, 17);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_PRINT, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_SCOPE, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_ELSEIF, 17);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_FOR, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_NEXT, 17);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_STATIC, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_IDENTIFIER, 16);
	TABLE_SET(NT_STMT_SEQ, TOKEN_EOL, 16);
	// VAR_DECL
	TABLE_SET(NT_VAR_DECL, TOKEN_KW_DIM, 18);
	TABLE_SET(NT_VAR_DECL, TOKEN_KW_STATIC, 19);
	// VAR_DECL_NEXT
	TABLE_SET(NT_VAR_DECL_NEXT, TOKEN_KW_SHARED, 21);
	TABLE_SET(NT_VAR_DECL_NEXT, TOKEN_IDENTIFIER, 20);
	// VAR_DEF
	TABLE_SET(NT_VAR_DEF, TOKEN_IDENTIFIER, 22);
	// INIT_OPT
	TABLE_SET(NT_INIT_OPT, TOKEN_EOL, 24);
	TABLE_SET(NT_INIT_OPT, TOKEN_EQUAL, 23);
	TABLE_SET(NT_INIT_OPT, TOKEN_EOF, 24);
	// FUNC_DECL
	TABLE_SET(NT_FUNC_DECL, TOKEN_KW_DECLARE, 25);
	// TYPE
	TABLE_SET(NT_TYPE, TOKEN_KW_DOUBLE, 27);
	TABLE_SET(NT_TYPE, TOKEN_KW_INTEGER, 26);
	TABLE_SET(NT_TYPE, TOKEN_KW_STRING, 28);
	TABLE_SET(NT_TYPE, TOKEN_KW_BOOLEAN, 29);
	// FUNC_DEF
	TABLE_SET(NT_FUNC_DEF, TOKEN_KW_FUNCTION, 30);
	// FUNC_BODY
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_DIM, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_DO, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_AND, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_IF, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_INPUT, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_PRINT, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_RETURN, 32);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_SCOPE, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_FOR, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_KW_STATIC, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_IDENTIFIER, 31);
	TABLE_SET(NT_FUNC_BODY, TOKEN_EOL, 31);
	// PARAM_DECL
	TABLE_SET(NT_PARAM_DECL, TOKEN_IDENTIFIER, 33);
	// PARAMS
	TABLE_SET(NT_PARAMS, TOKEN_IDENTIFIER, 34);
	TABLE_SET(NT_PARAMS, TOKEN_RPAR, 35);
	// PARAMS_NEXT
	TABLE_SET(NT_PARAMS_NEXT, TOKEN_COMMA, 36);
	TABLE_SET(NT_PARAMS_NEXT, TOKEN_RPAR, 37);
	// ASSIGNMENT
	TABLE_SET(NT_ASSIGNMENT, TOKEN_IDENTIFIER, 38);
	// INPUT_STMT
	TABLE_SET(NT_INPUT_STMT, TOKEN_KW_INPUT, 39);
	// PRINT_STMT
	TABLE_SET(NT_PRINT_STMT, TOKEN_KW_PRINT, 40);
	// EXPRESSION_LIST
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_KW_FALSE, 41);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_KW_TRUE, 41);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_STRING, 41);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_INT, 41);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_REAL, 41);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_IDENTIFIER, 41);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_EOL, 42);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_EOF, 42);
	// SCOPE_STMT
	TABLE_SET(NT_SCOPE_STMT, TOKEN_KW_SCOPE, 43);
	// IF_STMT
	TABLE_SET(NT_IF_STMT, TOKEN_KW_IF, 44);
	// IF_STMT_CONT
	TABLE_SET(NT_IF_STMT_CONT, TOKEN_KW_ELSE, 46);
	TABLE_SET(NT_IF_STMT_CONT, TOKEN_KW_AND, 45);
	TABLE_SET(NT_IF_STMT_CONT, TOKEN_KW_ELSEIF, 47);
	// IF_STMT_END
	TABLE_SET(NT_IF_STMT_END, TOKEN_KW_ELSE, 49);
	TABLE_SET(NT_IF_STMT_END, TOKEN_KW_END, 48);
	// DO_STMT
	TABLE_SET(NT_DO_STMT, TOKEN_KW_DO, 50);
	// DO_STMT_END
	TABLE_SET(NT_DO_STMT_END, TOKEN_KW_WHILE, 51);
	TABLE_SET(NT_DO_STMT_END, TOKEN_KW_UNTIL, 51);
	TABLE_SET(NT_DO_STMT_END, TOKEN_EOL, 52);
	// TEST_TYPE
	TABLE_SET(NT_TEST_TYPE, TOKEN_KW_WHILE, 53);
	TABLE_SET(NT_TEST_TYPE, TOKEN_KW_UNTIL, 54);
	// DO_BODY
	TABLE_SET(NT_DO_BODY, TOKEN_KW_DIM, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_DO, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_IF, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_INPUT, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_LOOP, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_PRINT, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_SCOPE, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_CONTINUE, 57);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_EXIT, 56);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_FOR, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_KW_STATIC, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_IDENTIFIER, 55);
	TABLE_SET(NT_DO_BODY, TOKEN_EOL, 55);
	// FOR_STMT
	TABLE_SET(NT_FOR_STMT, TOKEN_KW_FOR, 58);
	// FOR_BODY
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_DIM, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_DO, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_IF, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_INPUT, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_PRINT, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_SCOPE, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_CONTINUE, 61);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_EXIT, 60);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_FOR, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_NEXT, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_KW_STATIC, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_IDENTIFIER, 59);
	TABLE_SET(NT_FOR_BODY, TOKEN_EOL, 59);
	// TYPE_OPT
	TABLE_SET(NT_TYPE_OPT, TOKEN_KW_AS, 62);
	TABLE_SET(NT_TYPE_OPT, TOKEN_EQUAL, 63);
	// STEP_OPT
	TABLE_SET(NT_STEP_OPT, TOKEN_KW_STEP, 64);
	TABLE_SET(NT_STEP_OPT, TOKEN_EOL, 65);
	// ID_OPT
	TABLE_SET(NT_ID_OPT, TOKEN_IDENTIFIER, 66);
	TABLE_SET(NT_ID_OPT, TOKEN_EOL, 67);
	TABLE_SET(NT_ID_OPT, TOKEN_EOF, 67);
	// ASSIGN_OPERATOR
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_EQUAL, 68);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_SUB_ASIGN, 69);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_ADD_ASIGN, 70);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_MUL_ASIGN, 71);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_DIVI_ASIGN, 72);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_DIVR_ASIGN, 73);
	// EXPRESSION
	TABLE_SET(NT_EXPRESSION, TOKEN_KW_FALSE, 79);
	TABLE_SET(NT_EXPRESSION, TOKEN_KW_TRUE, 78);
	TABLE_SET(NT_EXPRESSION, TOKEN_STRING, 77);
	TABLE_SET(NT_EXPRESSION, TOKEN_INT, 75);
	TABLE_SET(NT_EXPRESSION, TOKEN_REAL, 76);
	TABLE_SET(NT_EXPRESSION, TOKEN_IDENTIFIER, 74);



	return true;
}

void grammar_free() {
	for (int i = 0; i < NUM_OF_RULES; i++) {
		rule_free(grammar.rules[i]);
	}

	sparse_table_free(grammar.LL_table);
}
