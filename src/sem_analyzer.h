#ifndef IFJ17_COMPILER_SEM_ANALYZER_H
#define IFJ17_COMPILER_SEM_ANALYZER_H

#include <stdbool.h>
#include "dllist.h"

#define SEM_VALUE_TOKEN(value) ((SemValue){.value_type = VTYPE_TOKEN, {.token = (value)}})

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
struct htab_item;


typedef enum {
    VTYPE_TOKEN,
    VTYPE_ID,
    VTYPE_LIST
} value_type_e;

typedef struct sem_value_t {
    value_type_e value_type;
    union {
        struct token_t* token;
        struct htab_item* id;
        DLList* list;
    };
} SemValue;

/**
 * Function that represents semantic action
 *
 * Takes care of freeing token if it does not use it
 *
 * @return return code (see error_code.h)
 */
typedef int (*semantic_action_f) (struct sem_analyzer_t*, struct parser_t*, SemValue);

/**
 * Semantic analyzer object, holds current state of semantic analysis
 */
typedef struct sem_analyzer_t {
    semantic_action_f sem_action;  /// Semantic action
    bool finished;  /// Indicates whether semantic action is finished or not
    sem_state_e state;  /// State of semantic action
    SemValue* value;  /// Constant or temporary variable from nested SemAnalyzer or aggregated value for parent SymAnalyzer
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

/**
 * Deep copy semantic value, but only move VTYPE_LIST
 * @param value semantic value
 * @return deep copy of value, NULL on error
 */
SemValue* sem_value_copy(const SemValue* value);

/**
 * Free semantic value
 * @param value SemValue
 */
void sem_value_free(void* value);


// SEMANTIC FUNCTIONS

int sem_var_decl(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);

#endif //IFJ17_COMPILER_SEM_ANALYZER_H
