/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_GRAMMAR_H
#define IFJ17_COMPILER_GRAMMAR_H

#include <stdbool.h>
#include "sparse_table.h"
#include "sem_analyzer.h"

// Number of rules in grammar. Needs to be incremented by 1 because first rule is empty
#define NUM_OF_RULES 79

/// Get number of variable arguments
#define NUM_ARGS(...)  (sizeof((unsigned[]){__VA_ARGS__})/sizeof(unsigned))

#define IS_NONTERMINAL(expr) ((expr) < NT_ENUM_SIZE)

/**
 * Enum of non terminals.
 *
 * Special enum constants:
 * NT_ENUM_SIZE - number of non terminals, size of grammar array
 * END_OF_RULE - used to determine the end of rule production array
 * TERMINALS_START - starting point of terminals (tokens) see {@link token.h}, used to determine terminals from non-terminals
 */
typedef enum {
    NT_LINE,
    NT_LINE_END,
    NT_GLOBAL_STMT,
    NT_INNER_STMT,
    NT_STMT_SEQ,
    NT_VAR_DECL,
    NT_SHARED_VAR,
    NT_VAR_DEF,
    NT_INIT_OPT,
    NT_FUNC_DECL,
    NT_TYPE,
    NT_FUNC_DEF,
    NT_PARAM_DECL,
    NT_PARAMS,
    NT_PARAMS_NEXT,
    NT_RETURN_STMT,
    NT_ASSIGNMENT,
    NT_INPUT_STMT,
    NT_PRINT_STMT,
    NT_EXPRESSION_LIST,
    NT_SCOPE_STMT,
    NT_IF_STMT,
    NT_IF_STMT_ELSEIF,
    NT_IF_STMT_ELSE,
    NT_DO_STMT,
    NT_DO_STMT_END,
    NT_TEST_TYPE_START,
    NT_TEST_TYPE_END,
    NT_EXIT_STMT,
    NT_CONTINUE_STMT,
    NT_LOOP_TYPE,
    NT_LOOP_TYPE_END,
    NT_FOR_STMT,
    NT_TYPE_OPT,
    NT_STEP_OPT,
    NT_ID_OPT,
    NT_ASSIGN_OPERATOR,
    NT_EXPRESSION,    // Nonterminal for operator-precedence grammar
    NT_LIST,    // Nonterminal for operator-precedence grammar

    // Special enum items
    NT_ENUM_SIZE,
    END_OF_RULE,
    TERMINALS_START
} non_terminal_e;

/**
 * Grammar rule structure
 */
typedef struct rule_t {
    non_terminal_e for_nt;  /// To which non terminal this rule applies
    unsigned* production;  /// Array of non_terminal_e and token_e ending with END_OF_RULE
    semantic_action_f sem_action;  /// Semantic action, NULL if no action
} Rule;

/**
 * Global grammar. Needs to be initialized.
 */
extern struct grammar_t {
    Rule* rules[NUM_OF_RULES];  /// Array of rules.
    SparseTable* LL_table;  /// The LL table of the grammar
} grammar;

/**
 * Initialize grammar
 */
void grammar_init();

/**
 * Free rule
 * @param rule pointer to rule structure
 */
void rule_free(Rule* rule);

/**
 * Free grammar
 */
void grammar_free();

#endif //IFJ17_COMPILER_GRAMMAR_H
