#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include "grammar.h"
#include "token.h"

/// Add epsilon rule to grammar, cleanup on failure
#define ADD_EPSILON_RULE(nt)  if (!grammar_add_epsilon_rule(curr_idx++, nt)) { grammar_free(); return false; }
/// Add rule to grammar, cleanup on failure
#define ADD_RULE(nt, ...) if (!grammar_add_rule(curr_idx++, nt, NULL, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)) { grammar_free(); return false; }
/// Add rule with semantic action, cleanup on failure
#define ADD_SEMANTIC_RULE(nt, sem_action, ...) if (!grammar_add_rule(curr_idx++, nt, sem_action, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)) { grammar_free(); return false; }

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
	rule->sem_action = NULL;

	grammar.rules[idx] = rule;

	return true;
}

static bool grammar_add_rule(int idx, non_terminal_e nt, semantic_action_f sem_action, int va_num, ...) {
	assert(idx < NUM_OF_RULES);

	Rule* rule = (Rule*) malloc(sizeof(Rule));
	if (rule == NULL) {
		return false;
	}


	rule->production = (unsigned*) malloc(sizeof(unsigned) * (va_num + 1));

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
	rule->sem_action = sem_action;

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

	ADD_RULE(NT_LINE, NT_GLOBAL_STMT, NT_SCOPE_STMT, NT_LINE_END);
	ADD_EPSILON_RULE(NT_LINE);
	ADD_RULE(NT_LINE_END, TOKEN_EOL, NT_LINE_END);
	ADD_EPSILON_RULE(NT_LINE_END);
	ADD_RULE(NT_GLOBAL_STMT, NT_FUNC_DECL, TOKEN_EOL, NT_GLOBAL_STMT);
	ADD_RULE(NT_GLOBAL_STMT, NT_FUNC_DEF, TOKEN_EOL, NT_GLOBAL_STMT);
	ADD_RULE(NT_GLOBAL_STMT, NT_SHARED_VAR, TOKEN_EOL, NT_GLOBAL_STMT);
	ADD_RULE(NT_GLOBAL_STMT, TOKEN_EOL, NT_GLOBAL_STMT);
	ADD_EPSILON_RULE(NT_GLOBAL_STMT);
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
	ADD_SEMANTIC_RULE(NT_VAR_DECL, sem_var_decl, TOKEN_KW_DIM, NT_VAR_DEF);
	ADD_RULE(NT_VAR_DECL, TOKEN_KW_STATIC, NT_VAR_DEF);
	ADD_RULE(NT_SHARED_VAR, TOKEN_KW_DIM, TOKEN_KW_SHARED, NT_VAR_DEF);
	ADD_RULE(NT_VAR_DEF, TOKEN_IDENTIFIER, TOKEN_KW_AS, NT_TYPE, NT_INIT_OPT);
	ADD_RULE(NT_INIT_OPT, TOKEN_EQUAL, NT_EXPRESSION);
	ADD_EPSILON_RULE(NT_INIT_OPT);
	ADD_SEMANTIC_RULE(NT_FUNC_DECL, sem_func_decl, TOKEN_KW_DECLARE, TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, NT_PARAMS, TOKEN_RPAR, TOKEN_KW_AS, NT_TYPE);
	ADD_RULE(NT_TYPE, TOKEN_KW_INTEGER);
	ADD_RULE(NT_TYPE, TOKEN_KW_DOUBLE);
	ADD_RULE(NT_TYPE, TOKEN_KW_STRING);
	ADD_RULE(NT_TYPE, TOKEN_KW_BOOLEAN);
	ADD_SEMANTIC_RULE(NT_FUNC_DEF, sem_func_def, TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, NT_PARAMS, TOKEN_RPAR, TOKEN_KW_AS, NT_TYPE, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_END, TOKEN_KW_FUNCTION);
	ADD_SEMANTIC_RULE(NT_PARAM_DECL, sem_param_decl, TOKEN_IDENTIFIER, TOKEN_KW_AS, NT_TYPE);
	ADD_RULE(NT_PARAMS, NT_PARAM_DECL, NT_PARAMS_NEXT);
	ADD_EPSILON_RULE(NT_PARAMS);
	ADD_RULE(NT_PARAMS_NEXT, TOKEN_COMMA, NT_PARAM_DECL, NT_PARAMS_NEXT);
	ADD_EPSILON_RULE(NT_PARAMS_NEXT);
	ADD_SEMANTIC_RULE(NT_RETURN_STMT, sem_return, TOKEN_KW_RETURN, NT_EXPRESSION);
	ADD_SEMANTIC_RULE(NT_ASSIGNMENT, sem_expr_assign, TOKEN_IDENTIFIER, NT_ASSIGN_OPERATOR, NT_EXPRESSION);
	ADD_RULE(NT_INPUT_STMT, TOKEN_KW_INPUT, TOKEN_IDENTIFIER);
	ADD_SEMANTIC_RULE(NT_PRINT_STMT, sem_print, TOKEN_KW_PRINT, NT_EXPRESSION, TOKEN_SEMICOLON, NT_EXPRESSION_LIST);
	ADD_RULE(NT_EXPRESSION_LIST, NT_EXPRESSION, TOKEN_SEMICOLON, NT_EXPRESSION_LIST);
	ADD_EPSILON_RULE(NT_EXPRESSION_LIST);
	ADD_SEMANTIC_RULE(NT_SCOPE_STMT, sem_scope, TOKEN_KW_SCOPE, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_END, TOKEN_KW_SCOPE);
	ADD_SEMANTIC_RULE(NT_IF_STMT, sem_condition, TOKEN_KW_IF, NT_EXPRESSION, TOKEN_KW_THEN, TOKEN_EOL, NT_STMT_SEQ, NT_IF_STMT_ELSEIF, NT_IF_STMT_ELSE, TOKEN_KW_END, TOKEN_KW_IF);
	ADD_RULE(NT_IF_STMT_ELSEIF, TOKEN_KW_ELSEIF, NT_EXPRESSION, TOKEN_KW_THEN, TOKEN_EOL, NT_STMT_SEQ, NT_IF_STMT_ELSEIF);
	ADD_EPSILON_RULE(NT_IF_STMT_ELSEIF);
	ADD_RULE(NT_IF_STMT_ELSE, TOKEN_KW_ELSE, TOKEN_EOL, NT_STMT_SEQ);
	ADD_EPSILON_RULE(NT_IF_STMT_ELSE);
	ADD_SEMANTIC_RULE(NT_DO_STMT, sem_do_loop, TOKEN_KW_DO, NT_DO_STMT_END);
	ADD_RULE(NT_DO_STMT_END, NT_TEST_TYPE, NT_EXPRESSION, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_LOOP);
	ADD_RULE(NT_DO_STMT_END, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_LOOP, NT_TEST_TYPE, NT_EXPRESSION);
	ADD_RULE(NT_TEST_TYPE, TOKEN_KW_WHILE);
	ADD_RULE(NT_TEST_TYPE, TOKEN_KW_UNTIL);
	ADD_SEMANTIC_RULE(NT_EXIT_STMT, sem_exit, TOKEN_KW_EXIT, NT_LOOP_TYPE);
	ADD_SEMANTIC_RULE(NT_CONTINUE_STMT, sem_continue, TOKEN_KW_CONTINUE, NT_LOOP_TYPE);
	ADD_RULE(NT_LOOP_TYPE, TOKEN_KW_DO);
	ADD_RULE(NT_LOOP_TYPE, TOKEN_KW_FOR);
	ADD_SEMANTIC_RULE(NT_FOR_STMT, sem_for_loop, TOKEN_KW_FOR, TOKEN_IDENTIFIER, NT_TYPE_OPT, TOKEN_EQUAL, NT_EXPRESSION, TOKEN_KW_TO, NT_EXPRESSION, NT_STEP_OPT, TOKEN_EOL, NT_STMT_SEQ, TOKEN_KW_NEXT, NT_ID_OPT);
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

	// LL table initialization

	// LINE
	TABLE_SET(NT_LINE, TOKEN_KW_DECLARE, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_DIM, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_FUNCTION, 1);
	TABLE_SET(NT_LINE, TOKEN_KW_SCOPE, 1);
	TABLE_SET(NT_LINE, TOKEN_EOL, 1);
	TABLE_SET(NT_LINE, TOKEN_EOF, 2);
	// LINE_END
	TABLE_SET(NT_LINE_END, TOKEN_EOL, 3);
	TABLE_SET(NT_LINE_END, TOKEN_EOF, 4);
	// GLOBAS_STMT
	TABLE_SET(NT_GLOBAL_STMT, TOKEN_KW_DECLARE, 5);
	TABLE_SET(NT_GLOBAL_STMT, TOKEN_KW_DIM, 7);
	TABLE_SET(NT_GLOBAL_STMT, TOKEN_KW_FUNCTION, 6);
	TABLE_SET(NT_GLOBAL_STMT, TOKEN_KW_SCOPE, 9);
	TABLE_SET(NT_GLOBAL_STMT, TOKEN_EOL, 8);
	// INNER_STMT
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_DIM, 10);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_DO, 14);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_IF, 12);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_INPUT, 17);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_PRINT, 16);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_RETURN, 18);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_SCOPE, 13);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_CONTINUE, 20);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_EXIT, 19);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_FOR, 15);
	TABLE_SET(NT_INNER_STMT, TOKEN_KW_STATIC, 10);
	TABLE_SET(NT_INNER_STMT, TOKEN_IDENTIFIER, 11);
	TABLE_SET(NT_INNER_STMT, TOKEN_EOL, 21);
	// STMT_SEQ
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_DIM, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_DO, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_ELSE, 23);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_END, 23);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_IF, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_INPUT, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_LOOP, 23);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_PRINT, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_RETURN, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_SCOPE, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_CONTINUE, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_ELSEIF, 23);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_EXIT, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_FOR, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_NEXT, 23);
	TABLE_SET(NT_STMT_SEQ, TOKEN_KW_STATIC, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_IDENTIFIER, 22);
	TABLE_SET(NT_STMT_SEQ, TOKEN_EOL, 22);
	// VAR_DECL
	TABLE_SET(NT_VAR_DECL, TOKEN_KW_DIM, 24);
	TABLE_SET(NT_VAR_DECL, TOKEN_KW_STATIC, 25);
	// SHARED_VAR
	TABLE_SET(NT_SHARED_VAR, TOKEN_KW_DIM, 26);
	// VAR_DEF
	TABLE_SET(NT_VAR_DEF, TOKEN_IDENTIFIER, 27);
	// INIT_OPT
	TABLE_SET(NT_INIT_OPT, TOKEN_EOL, 29);
	TABLE_SET(NT_INIT_OPT, TOKEN_EQUAL, 28);
	// FUNC_DECL
	TABLE_SET(NT_FUNC_DECL, TOKEN_KW_DECLARE, 30);
	// TYPE
	TABLE_SET(NT_TYPE, TOKEN_KW_DOUBLE, 32);
	TABLE_SET(NT_TYPE, TOKEN_KW_INTEGER, 31);
	TABLE_SET(NT_TYPE, TOKEN_KW_STRING, 33);
	TABLE_SET(NT_TYPE, TOKEN_KW_BOOLEAN, 34);
	// FUNC_DEF
	TABLE_SET(NT_FUNC_DEF, TOKEN_KW_FUNCTION, 35);
	// PARAM_DECL
	TABLE_SET(NT_PARAM_DECL, TOKEN_IDENTIFIER, 36);
	// PARAMS
	TABLE_SET(NT_PARAMS, TOKEN_IDENTIFIER, 37);
	TABLE_SET(NT_PARAMS, TOKEN_RPAR, 38);
	// PARAMS_NEXT
	TABLE_SET(NT_PARAMS_NEXT, TOKEN_COMMA, 39);
	TABLE_SET(NT_PARAMS_NEXT, TOKEN_RPAR, 40);
	// RETURN_STMT
	TABLE_SET(NT_RETURN_STMT, TOKEN_KW_RETURN, 41);
	// ASSIGNMENT
	TABLE_SET(NT_ASSIGNMENT, TOKEN_IDENTIFIER, 42);
	// INPUT_STMT
	TABLE_SET(NT_INPUT_STMT, TOKEN_KW_INPUT, 43);
	// PRINT_STMT
	TABLE_SET(NT_PRINT_STMT, TOKEN_KW_PRINT, 44);
	// EXPRESSION_LIST
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_KW_FALSE, 45);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_KW_TRUE, 45);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_STRING, 45);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_INT, 45);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_REAL, 45);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_IDENTIFIER, 45);
	TABLE_SET(NT_EXPRESSION_LIST, TOKEN_EOL, 46);
	// SCOPE_STMT
	TABLE_SET(NT_SCOPE_STMT, TOKEN_KW_SCOPE, 47);
	// IF_STMT
	TABLE_SET(NT_IF_STMT, TOKEN_KW_IF, 48);
	// IF_STMT_ELSEIF
	TABLE_SET(NT_IF_STMT_ELSEIF, TOKEN_KW_ELSE, 50);
	TABLE_SET(NT_IF_STMT_ELSEIF, TOKEN_KW_END, 50);
	TABLE_SET(NT_IF_STMT_ELSEIF, TOKEN_KW_ELSEIF, 49);
	// IF_STMT_ELSE
	TABLE_SET(NT_IF_STMT_ELSE, TOKEN_KW_ELSE, 51);
	TABLE_SET(NT_IF_STMT_ELSE, TOKEN_KW_END, 52);
	// DO_STMT
	TABLE_SET(NT_DO_STMT, TOKEN_KW_DO, 53);
	// DO_STMT_END
	TABLE_SET(NT_DO_STMT_END, TOKEN_KW_WHILE, 54);
	TABLE_SET(NT_DO_STMT_END, TOKEN_KW_UNTIL, 54);
	TABLE_SET(NT_DO_STMT_END, TOKEN_EOL, 55);
	// TEST_TYPE
	TABLE_SET(NT_TEST_TYPE, TOKEN_KW_WHILE, 56);
	TABLE_SET(NT_TEST_TYPE, TOKEN_KW_UNTIL, 57);
	// EXIT_STMT
	TABLE_SET(NT_EXIT_STMT, TOKEN_KW_EXIT, 58);
	// CONTINUE_STMT
	TABLE_SET(NT_CONTINUE_STMT, TOKEN_KW_CONTINUE, 59);
	// LOOP_TYPE
	TABLE_SET(NT_LOOP_TYPE, TOKEN_KW_DO, 60);
	TABLE_SET(NT_LOOP_TYPE, TOKEN_KW_FOR, 61);
	// FOR_STMT
	TABLE_SET(NT_FOR_STMT, TOKEN_KW_FOR, 62);
	// TYPE_OPT
	TABLE_SET(NT_TYPE_OPT, TOKEN_KW_AS, 63);
	TABLE_SET(NT_TYPE_OPT, TOKEN_EQUAL, 64);
	// STEP_OPT
	TABLE_SET(NT_STEP_OPT, TOKEN_KW_STEP, 65);
	TABLE_SET(NT_STEP_OPT, TOKEN_EOL, 66);
	// ID_OPT
	TABLE_SET(NT_ID_OPT, TOKEN_IDENTIFIER, 67);
	TABLE_SET(NT_ID_OPT, TOKEN_EOL, 68);
	// ASSIGN_OPERATOR
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_EQUAL, 69);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_SUB_ASIGN, 70);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_ADD_ASIGN, 71);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_MUL_ASIGN, 72);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_DIVI_ASIGN, 73);
	TABLE_SET(NT_ASSIGN_OPERATOR, TOKEN_DIVR_ASIGN, 74);



	return true;
}

void grammar_free() {
	for (int i = 0; i < NUM_OF_RULES; i++) {
		rule_free(grammar.rules[i]);
	}

	sparse_table_free(grammar.LL_table);
}
