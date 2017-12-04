/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "error_code.h"
#include "expr_grammar.h"
#include "expr_parser.h"
#include "3ac.h"
#include "buffer.h"
#include "debug.h"
#include "scanner.h"
#include "memory_manager.h"

#define RET_CODE_HANDLE_EXPRESSION 100

/**
 * Add built-in functions to HashTable
 * @param htab Hash table that stores function entries
 */
static void add_built_ins(HashTable* htab) {
	const char *built_in_func[] = { "length", "substr", "asc", "chr" };

	htab_item* built_in;
	for (int i = 0; i < 4; i++) {
		built_in = htab_func_insert(htab, built_in_func[i]);

		func_set_defined(built_in);
		// Set parameter types and return types
		switch (i) {
			// length
			case 0:
				func_set_ret_type(built_in, TOKEN_KW_INTEGER);
				func_add_param(built_in, TOKEN_KW_STRING);
				break;
			// substr
			case 1:
				func_set_ret_type(built_in, TOKEN_KW_STRING);
				func_add_param(built_in, TOKEN_KW_STRING);
				func_add_param(built_in, TOKEN_KW_INTEGER);
				func_add_param(built_in, TOKEN_KW_INTEGER);
				break;
			case 2:
				func_set_ret_type(built_in, TOKEN_KW_INTEGER);
				func_add_param(built_in, TOKEN_KW_STRING);
				func_add_param(built_in, TOKEN_KW_INTEGER);
				break;
			case 3:
				func_set_ret_type(built_in, TOKEN_KW_STRING);
				func_add_param(built_in, TOKEN_KW_INTEGER);
			default: break;
		}
	}

	// ASC function
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "asc"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "i"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "str"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "i"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "str"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_TMP, "i"), NO_ADDR);
	IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", "asc_noconvert"), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")));
	IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_TMP, "i"), addr_symbol(F_TMP, "i"), NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "asc_noconvert"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "strlength"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_SUB, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)));
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "strlength"), addr_symbol(F_LOCAL, "str"), NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if1cond"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "if1cond"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(0)));
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "wrongindex"), addr_symbol(F_LOCAL, "if1cond"), addr_constant(MAKE_TOKEN_BOOL(true)));
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if2cond"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_GT, addr_symbol(F_LOCAL, "if2cond"), addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "strlength"));
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "wrongindex"), addr_symbol(F_LOCAL, "if2cond"), addr_constant(MAKE_TOKEN_BOOL(true)));
	IL_ADD(func_il, OP_STRI2INT, addr_symbol(F_LOCAL, "retval"), addr_symbol(F_LOCAL, "str"), addr_symbol(F_LOCAL, "i"));
	IL_ADD(func_il, OP_JUMP, addr_symbol("", "ascvalue"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "wrongindex"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "retval"), addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "ascvalue"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_PUSHS, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR);

	// SUBSTR function
	IL_ADD_SPACE(func_il);
	IL_ADD_SPACE(func_il);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "substr"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "n"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "i"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "str"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "n"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "i"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "str"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_TMP, "n"), NO_ADDR);
	IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", "substr_n_noconvert"), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")));
	IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_TMP, "n"), addr_symbol(F_TMP, "n"), NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "substr_n_noconvert"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_TMP, "i"), NO_ADDR);
	IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", "substr_i_noconvert"), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")));
	IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_TMP, "i"), addr_symbol(F_TMP, "i"), NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "substr_i_noconvert"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "substr"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "anotherchar"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "counter"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "total"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "str"), NO_ADDR);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "substr"), addr_constant(MAKE_TOKEN_STRING("")), NO_ADDR);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "counter"), addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if1cond"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "if1cond"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)));
	IL_ADD(func_il, OP_SUB, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)));
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "finalstring"), addr_symbol(F_LOCAL, "if1cond"), addr_constant(MAKE_TOKEN_BOOL(true)));
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if2cond"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "if2cond"), addr_symbol(F_LOCAL, "n"), addr_constant(MAKE_TOKEN_INT(0)));
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "itoend"), addr_symbol(F_LOCAL, "if2cond"), addr_constant(MAKE_TOKEN_BOOL(true)));
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "if3cond"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_SUB, addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "i"));
	IL_ADD(func_il, OP_GT, addr_symbol(F_LOCAL, "if3cond"), addr_symbol(F_LOCAL, "n"), addr_symbol(F_LOCAL, "total"));
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "itoend"), addr_symbol(F_LOCAL, "if3cond"), addr_constant(MAKE_TOKEN_BOOL(true)));
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "charloop"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "finalstring"), addr_symbol(F_LOCAL, "counter"), addr_symbol(F_LOCAL, "n"));
	IL_ADD(func_il, OP_GETCHAR, addr_symbol(F_LOCAL, "anotherchar"), addr_symbol(F_LOCAL, "str"), addr_symbol(F_LOCAL, "i"));
	IL_ADD(func_il, OP_CONCAT, addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "anotherchar"));
	IL_ADD(func_il, OP_ADD, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)));
	IL_ADD(func_il, OP_ADD, addr_symbol(F_LOCAL, "counter"), addr_symbol(F_LOCAL, "counter"), addr_constant(MAKE_TOKEN_INT(1)));
	IL_ADD(func_il, OP_JUMP, addr_symbol("", "charloop"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "itoend"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "total"), addr_symbol(F_LOCAL, "str"), NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "itoendwhilecond"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "itoendwhile"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LT, addr_symbol(F_LOCAL, "itoendwhilecond"), addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "total"));
	IL_ADD(func_il, OP_JUMPIFEQ, addr_symbol("", "finalstring"), addr_symbol(F_LOCAL, "itoendwhilecond"), addr_constant(MAKE_TOKEN_BOOL(false)));
	IL_ADD(func_il, OP_GETCHAR, addr_symbol(F_LOCAL, "anotherchar"), addr_symbol(F_LOCAL, "str"), addr_symbol(F_LOCAL, "i"));
	IL_ADD(func_il, OP_CONCAT, addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "substr"), addr_symbol(F_LOCAL, "anotherchar"));
	IL_ADD(func_il, OP_ADD, addr_symbol(F_LOCAL, "i"), addr_symbol(F_LOCAL, "i"), addr_constant(MAKE_TOKEN_INT(1)));
	IL_ADD(func_il, OP_JUMP, addr_symbol("", "itoendwhile"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "finalstring"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_MOVE, addr_symbol(F_LOCAL, "retval"), addr_symbol(F_LOCAL, "substr"), NO_ADDR);
	IL_ADD(func_il, OP_PUSHS, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR);

	// LENGTH function
	IL_ADD_SPACE(func_il);
	IL_ADD_SPACE(func_il);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "length"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "sl"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "sl"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_TMP, "sl"), NO_ADDR);
	IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", "length_noconvert"), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")));
	IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_TMP, "i"), addr_symbol(F_TMP, "sl"), NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "length_noconvert"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_STRLEN, addr_symbol(F_LOCAL, "retval"), addr_symbol(F_LOCAL, "sl"), NO_ADDR);
	IL_ADD(func_il, OP_PUSHS, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR);

	// CHR function
	IL_ADD_SPACE(func_il);
	IL_ADD_SPACE(func_il);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "chr"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_TMP, "i"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPS, addr_symbol(F_TMP, "i"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_TMP, "i"), NO_ADDR);
	IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", "chr_noconvert"), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")));
	IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_TMP, "i"), addr_symbol(F_TMP, "i"), NO_ADDR);
	IL_ADD(func_il, OP_LABEL, addr_symbol("", "chr_noconvert"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_INT2CHAR, addr_symbol(F_LOCAL, "retval"), addr_symbol(F_LOCAL, "i"), NO_ADDR);
	IL_ADD(func_il, OP_PUSHS, addr_symbol(F_LOCAL, "retval"), NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR);
	IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR);
}

Parser* parser_init(Scanner* scanner) {
	Parser* parser = (Parser*) mm_malloc(sizeof(Parser));
	parser->scanner = scanner;
	grammar_init();
	expr_grammar_init();
	parser->dtree_stack = stack_init(30);
	parser->sym_tab_stack = dllist_init(htab_var_free);
	parser->sym_tab_global = htab_init(HTAB_INIT_SIZE);
	parser->sym_tab_functions = htab_init(HTAB_INIT_SIZE);
	add_built_ins(parser->sym_tab_functions);


	parser->sem_an_stack = dllist_init(sem_an_free);

	parser->il_override = NULL;
	parser->static_var_decl = false;
	parser->step_found = false;

	return parser;
}

void parser_free(Parser* parser) {
	assert(parser != NULL);

	htab_var_free(parser->sym_tab_global);
	dllist_free(parser->sym_tab_stack);
	dllist_free(parser->sem_an_stack);
	htab_func_free(parser->sym_tab_functions);
	grammar_free();
	expr_grammar_free();
	stack_free(parser->dtree_stack, NULL);
	mm_free(parser);
}

/**
 * Pass value from finished semantic analyzers to their parents
 * @param parser Parser
 * @return exit code
 */
static int handle_finished_semantics(Parser* parser) {
	SemAnalyzer* sem_an_to_free;
	int ret_code = EXIT_SUCCESS;

	SemAnalyzer* sem_an = (SemAnalyzer *) sem_stack_top(parser->sem_an_stack);

	// Finish up semantic analyzers
	while (sem_an != NULL && sem_an->finished && (ret_code == EXIT_SUCCESS)) {
		// If semantic action is finished, pop it from stack
		sem_stack_pop(parser->sem_an_stack);
		sem_an_to_free = sem_an;  // Store it for later freeing

		// Get parent semantic action
		sem_an = (SemAnalyzer *) sem_stack_top(parser->sem_an_stack);
		if (sem_an != NULL && sem_an_to_free->value != NULL) {
			// Call parent semantic action with value from child
			ret_code = sem_an->sem_action(sem_an, parser, *sem_an_to_free->value);
		}
		sem_an_free(sem_an_to_free);  // Free finished semantic analyzer
	}

	return ret_code;
}

/**
 * Handle finished semantics and pass current token to semantics if there are any left
 * @param parser Parser
 * @param token Current token
 * @return exit code
 */
static int handle_semantics(Parser *parser, Token *token) {
	int ret_code;

	ret_code = handle_finished_semantics(parser);

	if (!sem_stack_empty(parser->sem_an_stack) && ret_code == EXIT_SUCCESS) {

		SemAnalyzer *sem_an = (SemAnalyzer *) sem_stack_top(parser->sem_an_stack);
		ret_code = sem_an->sem_action(sem_an, parser, SEM_VALUE_TOKEN(token));
	}

	return ret_code;
}

/**
 * If rule has semantics, push it on stack of semantic analyzers
 * @param parser Parser
 * @param rule Rule
 */
static void add_rule_semantics(Parser* parser, Rule* rule) {
	if (rule->sem_action != NULL)
		sem_stack_push(parser->sem_an_stack, sem_an_init(rule->sem_action));
}

/**
 * Push rule production on derivation tree stack
 * @param parser Parser
 * @param rule Rule to apply
 */
static void rewrite_by_rule(Parser* parser, Rule* rule) {
	// Rewrite it to the rule production (rule production is already reversed)
	for (int i = 0; rule->production[i] != END_OF_RULE; i++) {
		stack_push(parser->dtree_stack, &rule->production[i]);
	}
}

/**
 * Get rule from LL table based on current derivation tree stack top and current token
 * @param stack_top Top of derivation tree stack
 * @param current_token Currently processed token
 * @return Rule to apply or NULL if no rule found
 */
static Rule* get_rule_from_ll_table(unsigned int stack_top, Token* current_token) {
	// Look at LL table to get index to rule with right production
	int rule_idx = sparse_table_get(grammar.LL_table, stack_top, get_token_column_value(current_token->id));

	// Get the rule from grammar
	return grammar.rules[rule_idx];
}

/**
 * Rewrite non terminals on top of derivation tree stack until there is terminal on top of the stack
 * @param parser Parser
 * @param token Currently processed token
 * @return exit code or RET_CODE_HANDLE_EXPRESSION indicating that expression parser should be called
 */
static int rewrite_until_terminal(Parser* parser, Token* token) {
	Rule* rule;  // Temp var for current rule

	// Look at what is on top of the stack
	unsigned int* s_top = (unsigned int*) stack_top(parser->dtree_stack);

	// If it is non terminal, rewrite it by rules, until there is terminal (token) in s_top
	while (*s_top < TERMINALS_START) {  // Non terminal loop
		if (*s_top == NT_EXPRESSION)
			return RET_CODE_HANDLE_EXPRESSION;

		rule = get_rule_from_ll_table(*s_top, token);

		// If no rule can be applied, return syntax error
		if (rule == NULL)
			return EXIT_SYNTAX_ERROR;

		add_rule_semantics(parser, rule);

		// Pop the current non terminal on top of stack
		stack_pop(parser->dtree_stack);

		rewrite_by_rule(parser, rule);
		// See what is now on top of the stack
		s_top = (unsigned int*) stack_top(parser->dtree_stack);
	}  // End non terminal loop

	return EXIT_SUCCESS;
}

int parse(Parser* parser) {
	assert(parser != NULL);

	int ret_code;

	// Push ending token and starting non terminal onto stack
	token_e eof_terminal = TOKEN_EOF;
	non_terminal_e start_non_terminal = NT_LINE;
	stack_push(parser->dtree_stack, &eof_terminal);
	stack_push(parser->dtree_stack, &start_non_terminal);

	Token* token = NULL;
	unsigned int* dtree_top;

	// Start processing tokens
	do {  // Token loop
		token_free(token);  // Free last token, does nothing when token is NULL

		// Get next token from scanner
		token = scanner_get_token(parser->scanner);
		if (token->id == LEX_ERROR) {  // Lexical error
			ret_code = EXIT_LEX_ERROR;
			break;
		}

		ret_code = rewrite_until_terminal(parser, token);

		if (ret_code == RET_CODE_HANDLE_EXPRESSION) {
			// Return token to buffer for expression parser
			scanner_unget_token(parser->scanner, token);
			token = NULL;
			// Call expression parser
			ret_code = parse_expression(parser);

			stack_pop(parser->dtree_stack);  // Pop expression non terminal from stack
		} else if (ret_code == EXIT_SUCCESS) {
			// Look at what is on top of the stack
			dtree_top = (unsigned int*) stack_top(parser->dtree_stack);
			// If the terminal (token) is the same as terminal on top of the stack and no error occurred, pop it from stack
			if (*dtree_top == token->id) {
				stack_pop(parser->dtree_stack);

				ret_code = handle_semantics(parser, token);
			} else {
				// Else there is syntax error
				ret_code = EXIT_SYNTAX_ERROR;
			}
		}
	} while ((token == NULL || token->id != TOKEN_EOF) && ret_code == EXIT_SUCCESS);  // End token loop

	if (ret_code != EXIT_SUCCESS) {
		debug("Error occured on line: %d\n", parser->scanner->line);
	}

	token_free(token);


	return ret_code;
}
