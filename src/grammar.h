#ifndef IFJ17_COMPILER_GRAMMAR_H
#define IFJ17_COMPILER_GRAMMAR_H

#include <stdbool.h>

typedef enum {
    NT_PROGRAM,
    NT_LINES_N,
    NT_LINE,
    NT_STATEMENT,
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
    NT_TEST_TYPE,
    NT_DO_BODY,
    NT_FOR_STMT,
    NT_FOR_BODY,
    NT_TYPE_OPT,
    NT_STEP_OPT,
    NT_ID_OPT,
    NT_ASSIGN_OPERATOR,
    NT_EXPRESSION,
    NT_ENUM_SIZE,
    END_OF_RULE,
    TERMINALS_START
} non_terminal_e;

typedef struct rule_t {
    int* production;  /// Array of non_terminal_e and token_e
    struct rule_t* next;  /// Next rule
} Rule;

Rule* grammar[NT_ENUM_SIZE];  /// Array of rules, values of non_terminal_e are indexes to this array. Needs to be initialized.

/**
 * Initialize grammar
 * @return true on success, false otherwise
 */
bool grammar_init();

/**
 * Free gramamar
 */
void grammar_free();

#endif //IFJ17_COMPILER_GRAMMAR_H
