#ifndef IFJ17_COMPILER_SEM_ANALYZER_H
#define IFJ17_COMPILER_SEM_ANALYZER_H

#include <stdbool.h>
#include "parser.h"
#include "token.h"

/**
 * Enumeration of semantic states
 */
typedef enum {
    SEM_STATE_START
} sem_state_e;

struct sem_analyzer_t;

/**
 * Function that represents semantic action
 *
 * @return return code (see error_code.h)
 */
typedef int (*semantic_action_f) (struct sem_analyzer_t*, Parser* parser, Token* token);

/**
 * Semantic analyzer object, holds current state of semantic analysis
 */
typedef struct sem_analyzer_t {
    semantic_action_f sem_action;  /// Semantic action
    sem_state_e state;  /// State of semantic action
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
void sem_an_free(SemAnalyzer* sem_an);

#endif //IFJ17_COMPILER_SEM_ANALYZER_H
