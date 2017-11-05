#ifndef IFJ17_COMPILER_GRAMMAR_H
#define IFJ17_COMPILER_GRAMMAR_H

#include <stdbool.h>
#include "sparse_table.h"

// Number of rules in grammar. Needs to be incremented by 1 because first rule is empty
#define NUM_OF_RULES 78

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
    NT_STATEMENT,
    NT_INNER_STMT,
    NT_STMT_SEQ,
    NT_VAR_DECL,
    NT_VAR_DECL_NEXT,
    NT_VAR_DEF,
    NT_INIT_OPT,
    NT_FUNC_DECL,
    NT_TYPE,
    NT_FUNC_DEF,
    NT_FUNC_BODY,
    NT_PARAMS,
    NT_PARAM_DECL,
    NT_PARAMS_NEXT,
    NT_ASSIGNMENT,
    NT_INPUT_STMT,
    NT_PRINT_STMT,
    NT_EXPRESSION_LIST,
    NT_SCOPE_STMT,
    NT_IF_STMT,
    NT_IF_STMT_CONT,
    NT_IF_STMT_END,
    NT_DO_STMT,
    NT_DO_STMT_END,
    NT_TEST_TYPE,
    NT_DO_BODY,
    NT_FOR_STMT,
    NT_FOR_BODY,
    NT_TYPE_OPT,
    NT_STEP_OPT,
    NT_ID_OPT,
    NT_ASSIGN_OPERATOR,
    NT_EXPRESSION,

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
    int* production;  /// Array of non_terminal_e and token_e ending with END_OF_RULE
} Rule;

/**
 * Global grammar. Needs to be initialized.
 */
struct grammar_t {
    Rule* rules[NUM_OF_RULES];  /// Array of rules.
    SparseTable* LL_table;  /// The LL table of the grammar
} grammar;

/**
 * Initialize grammar
 * @return true on success, false otherwise
 */
bool grammar_init();

/**
 * Free grammar
 */
void grammar_free();

#endif //IFJ17_COMPILER_GRAMMAR_H
