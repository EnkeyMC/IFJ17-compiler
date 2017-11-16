#ifndef IFJ17_COMPILER_SEM_ANALYZER_H
#define IFJ17_COMPILER_SEM_ANALYZER_H

#include <stdbool.h>

/**
 * Enumeration of semantic states
 */
typedef enum {
    SEM_STATE_START,
    SEM_STATE_VAR_TYPE,
    SEM_STATE_EOL
} sem_state_e;

// Forward declarations
struct sem_analyzer_t;
struct parser_t;
struct token_t;

/**
 * Function that represents semantic action
 *
 * Takes care of freeing token if it does not use it
 *
 * @return return code (see error_code.h)
 */
typedef int (*semantic_action_f) (struct sem_analyzer_t*, struct parser_t* parser, struct token_t* token);

/**
 * Semantic analyzer object, holds current state of semantic analysis
 */
typedef struct sem_analyzer_t {
    semantic_action_f sem_action;  /// Semantic action
    bool finished;  /// Indicates whether semantic action is finished or not
    sem_state_e state;  /// State of semantic action
    struct token_t* symbol;  /// Constant or temporary variable from nested SemAnalyzer or aggregated value for parent SymAnalyzer
} SemAnalyzer;

/**
 * Initialize semantic analyzer with sem_action and SEM_STATE_START state
 * @param sem_action Semantic action function
 * @return new SemAnalyzer object, NULL on error
 */
SemAnalyzer* sem_an_init(semantic_action_f sem_action);

/**
 * Free semantic action
 * @param sem_an SemAnalyzer
 */
void sem_an_free(void* sem_an);


// SEMANTIC FUNCTIONS

int sem_var_decl(SemAnalyzer* sem_an, struct parser_t* parser, struct token_t* token);

#endif //IFJ17_COMPILER_SEM_ANALYZER_H