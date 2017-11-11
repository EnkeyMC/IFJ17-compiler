#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include "grammar.h"
#include "token.h"

/// Add epsilon rule to grammar, cleanup on failure
#define ADD_EPSILON_RULE(nt)  if (!grammar_add_epsilon_rule(curr_idx++, nt)) { grammar_free(); return false; }
/// Add rule to grammar, cleanup on failure
#define ADD_RULE(nt, ...) if (!grammar_add_rule(curr_idx++, nt, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)) { grammar_free(); return false; }

/// Set table value, cleanup on failure
#define TABLE_SET(row, column, value) if (!sparse_table_set(grammar.LL_table, row, get_token_column_value(column), value)) { grammar_free(); return false; }


struct grammar_t grammar;


static void array_reverse(unsigned* array, int length) {
	unsigned tmp;
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

	rule->production = (unsigned*) malloc(sizeof(unsigned));
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

	array_reverse(rule->production, va_num);  // We are going to push it on stack in reverse order
	rule->production[i] = END_OF_RULE;
	rule->for_nt = nt;

	va_end(va_args);

	grammar.rules[idx] = rule;

	return true;
}

void rule_free(Rule* rule) {
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
	ADD_RULE(NT_INNER_STMT, NT_RETURN_STMT);
	ADD_RULE(NT_INNER_STMT, NT_EXIT_STMT);
	ADD_RULE(NT_INNER_STMT, NT_CONTINUE_STMT);
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
	ADD_RULE(NT_FUNC_DEF, TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, NT_PARAMS, TOKEN_RPAR, TOKEN_KW_AS, NT_TYPE, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_END, TOKEN_KW_FUNCTION);
	ADD_RULE(NT_PARAM_DECL, TOKEN_IDENTIFIER, TOKEN_KW_AS, NT_TYPE);
	ADD_RULE(NT_PARAMS, NT_PARAM_DECL, NT_PARAMS_NEXT);
	ADD_EPSILON_RULE(NT_PARAMS);
	ADD_RULE(NT_PARAMS_NEXT, TOKEN_COMMA, NT_PARAM_DECL, NT_PARAMS_NEXT);
	ADD_EPSILON_RULE(NT_PARAMS_NEXT);
	ADD_RULE(NT_RETURN_STMT, TOKEN_KW_RETURN, NT_EXPRESSION);
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
	ADD_RULE(NT_DO_STMT_END, NT_TEST_TYPE, NT_EXPRESSION, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_LOOP);
	ADD_RULE(NT_DO_STMT_END, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_LOOP, NT_TEST_TYPE, NT_EXPRESSION);
	ADD_RULE(NT_TEST_TYPE, TOKEN_KW_WHILE);
	ADD_RULE(NT_TEST_TYPE, TOKEN_KW_UNTIL);
	ADD_RULE(NT_EXIT_STMT, TOKEN_KW_EXIT, NT_LOOP_TYPE);
	ADD_RULE(NT_CONTINUE_STMT, TOKEN_KW_CONTINUE, NT_LOOP_TYPE);
	ADD_RULE(NT_LOOP_TYPE, TOKEN_KW_DO);
	ADD_RULE(NT_LOOP_TYPE, TOKEN_KW_FOR);
	ADD_RULE(NT_FOR_STMT, TOKEN_KW_FOR, TOKEN_IDENTIFIER, NT_TYPE_OPT, TOKEN_EQUAL, NT_EXPRESSION, TOKEN_KW_TO, NT_EXPRESSION, NT_STEP_OPT, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_NEXT, NT_ID_OPT);
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
	TABLE_SET(NT_LINE, TOKEN_KW_RETURN, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_SCOPE, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_CONTINUE, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_EXIT, 1);
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
	TABLE_SET(NT_STATEMENT, TOKEN_KW_RETURN, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_SCOPE, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_CONTINUE, 4);
	TABLE_SET(NT_STATEMENT, TOKEN_KW_EXIT, 4);
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
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_RETURN, 15);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_SCOPE, 10);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_CONTINUE, 17);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_EXIT, 16);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_FOR, 12);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_STATIC, 7);
	TABLE_SET(NT_INNER_STMT, TOKEN_IDENTIFIER, 8);
	TABLE_SET(NT_INNER_STMT, TOKEN_EOL, 18);
	TABLE_SET(NT_INNER_STMT, TOKEN_EOF, 18);
	// STMT_SEQ
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_DIM, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_DO, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_ELSE, 20);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_END, 20);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_IF, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_INPUT, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_LOOP, 20);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_PRINT, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_RETURN, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_SCOPE, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_CONTINUE, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_ELSEIF, 20);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_EXIT, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_FOR, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_NEXT, 20);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_STATIC, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_IDENTIFIER, 19);
	TABLE_SET(NT_STMT_SEQ, TOKEN_EOL, 19);
	// VAR_DECL
	TABLE_SET(NT_VAR_DECL, TOKEN_KW_DIM, 21);
	TABLE_SET(NT_VAR_DECL, TOKEN_KW_STATIC, 22);
	// VAR_DECL_NEXT
	TABLE_SET(NT_VAR_DECL_NEXT, TOKEN_KW_SHARED, 24);
	TABLE_SET(NT_VAR_DECL_NEXT, TOKEN_IDENTIFIER, 23);
	// VAR_DEF
	TABLE_SET(NT_VAR_DEF, TOKEN_IDENTIFIER, 25);
	// INIT_OPT
	TABLE_SET(NT_INIT_OPT, TOKEN_EOL, 27);
	TABLE_SET(NT_INIT_OPT, TOKEN_EQUAL, 26);
	TABLE_SET(NT_INIT_OPT, TOKEN_EOF, 27);
	// FUNC_DECL
	TABLE_SET(NT_FUNC_DECL, TOKEN_KW_DECLARE, 28);
	// TYPE
	TABLE_SET(NT_TYPE, TOKEN_KW_DOUBLE, 30);
	TABLE_SET(NT_TYPE, TOKEN_KW_INTEGER, 29);
	TABLE_SET(NT_TYPE, TOKEN_KW_STRING, 31);
	TABLE_SET(NT_TYPE, TOKEN_KW_BOOLEAN, 32);
	// FUNC_DEF
	TABLE_SET(NT_FUNC_DEF, TOKEN_KW_FUNCTION, 33);
	// PARAM_DECL
	TABLE_SET(NT_PARAM_DECL, TOKEN_IDENTIFIER, 34);
	// PARAMS
	TABLE_SET(NT_PARAMS, TOKEN_IDENTIFIER, 35);
	TABLE_SET(NT_PARAMS, TOKEN_RPAR, 36);
	// PARAMS_NEXT
	TABLE_SET(NT_PARAMS_NEXT, TOKEN_COMMA, 37);
	TABLE_SET(NT_PARAMS_NEXT, TOKEN_RPAR, 38);
	// RETURN_STMT
	TABLE_SET(NT_RETURN_STMT, TOKEN_KW_RETURN, 39);
	// ASSIGNMENT
	TABLE_SET(NT_ASSIGNMENT, TOKEN_IDENTIFIER, 40);
	// INPUT_STMT
	TABLE_SET(NT_INPUT_STMT, TOKEN_KW_INPUT, 41);
	// PRINT_STMT
	TABLE_SET(NT_PRINT_STMT, TOKEN_KW_PRINT, 42);
	// EXPRESSION_LIST
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_KW_FALSE, 43);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_KW_TRUE, 43);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_STRING, 43);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_INT, 43);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_REAL, 43);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_IDENTIFIER, 43);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_EOL, 44);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_EOF, 44);
	// SCOPE_STMT
	TABLE_SET(NT_SCOPE_STMT, TOKEN_KW_SCOPE, 45);
	// IF_STMT
	TABLE_SET(NT_IF_STMT, TOKEN_KW_IF, 46);
	// IF_STMT_CONT
	TABLE_SET(NT_IF_STMT_CONT, TOKEN_KW_ELSE, 48);
	TABLE_SET(NT_IF_STMT_CONT, TOKEN_KW_END, 47);
	TABLE_SET(NT_IF_STMT_CONT, TOKEN_KW_ELSEIF, 49);
	// IF_STMT_END
	TABLE_SET(NT_IF_STMT_END, TOKEN_KW_ELSE, 51);
	TABLE_SET(NT_IF_STMT_END, TOKEN_KW_END, 50);
	// DO_STMT
	TABLE_SET(NT_DO_STMT, TOKEN_KW_DO, 52);
	// DO_STMT_END
	TABLE_SET(NT_DO_STMT_END, TOKEN_KW_WHILE, 53);
	TABLE_SET(NT_DO_STMT_END, TOKEN_KW_UNTIL, 53);
	TABLE_SET(NT_DO_STMT_END, TOKEN_EOL, 54);
	// TEST_TYPE
	TABLE_SET(NT_TEST_TYPE, TOKEN_KW_WHILE, 55);
	TABLE_SET(NT_TEST_TYPE, TOKEN_KW_UNTIL, 56);
	// EXIT_STMT
	TABLE_SET(NT_EXIT_STMT, TOKEN_KW_EXIT, 57);
	// CONTINUE_STMT
	TABLE_SET(NT_CONTINUE_STMT, TOKEN_KW_CONTINUE, 58);
	// LOOP_TYPE
	TABLE_SET(NT_LOOP_TYPE, TOKEN_KW_DO, 59);
	TABLE_SET(NT_LOOP_TYPE, TOKEN_KW_FOR, 60);
	// FOR_STMT
	TABLE_SET(NT_FOR_STMT, TOKEN_KW_FOR, 61);
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
