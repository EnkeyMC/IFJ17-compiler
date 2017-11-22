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
	SEM_STATE_ASSIGN,
	SEM_STATE_FUNC_RETURN_TYPE,
	SEM_STATE_DECLARED_VAR_TYPE,
	SEM_STATE_DECLARED_RETURN_TYPE,
	SEM_STATE_FUNC_END,
	SEM_STATE_EOL,
	SEM_STATE_SCOPE_END,
	SEM_STATE_DO_LOOP,
	SEM_STATE_FOR_ITERATOR,
	SEM_STATE_FOR_INIT,
	SEM_STATE_FOR_ENDVAL,
	SEM_STATE_FOR_NEXT,
	SEM_STATE_FOR_END,
	SEM_STATE_OPERATOR,
	SEM_STATE_OPERAND,
	SEM_STATE_LIST,
	SEM_STATE_FUNC_ID
} sem_state_e;

// Forward declarations
struct sem_analyzer_t;
struct parser_t;
struct token_t;
struct htab_item_t;


typedef enum {
	VTYPE_TOKEN,
	VTYPE_ID,
	VTYPE_LIST,
	VTYPE_EXPR
} value_type_e;

typedef struct sem_value_t {
	value_type_e value_type;
	union {
		struct token_t* token;
		struct htab_item_t* id;
		DLList* list;
		int expr_type;	/// Should be token_e but can't include token.h
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
	bool finished;	/// Indicates whether semantic action is finished or not
	sem_state_e state;	/// State of semantic action
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
 * Initialize new semantic value
 * @return new SemValue, NULL on error
 */
SemValue* sem_value_init();

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

// ------------------------
// SEMANTIC STACK FUNCTIONS
// ------------------------

/**
 * Return whether stack is empty or not
 * @param s valid Stack object
 * @return true if empty, false otherwise
 */
bool sem_stack_empty(DLList *s);

/**
 * Return item on the top of the stack, does not remove it from the stack
 * @param s valid Stack object
 * @return item from top of the stack, NULL if stack is empty
 */
void* sem_stack_top(DLList *s);

/**
 * Return item on the top of the stack and remove it from the stack
 * @param s valid Stack object
 * @return item from top of the stack, NULL if stack is empty
 */
void* sem_stack_pop(DLList *s);

/**
 * Push item to a top of the stack
 * @param s valid Stack object
 * @param item to push to stack
 * @return true on success, false on allocation error
 */
bool sem_stack_push(DLList *s, void* item);

// ------------------
// SEMANTIC FUNCTIONS
// ------------------

int sem_expr_result(SemAnalyzer *sem_an, struct parser_t *parser, SemValue value);

int sem_expr_id(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_const(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_and_or_not(SemAnalyzer *sem_an, struct parser_t *parser, SemValue value);
int sem_expr_lte_gte(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_eq_ne(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_aritmetic_basic(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_div(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_brackets(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_unary(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_list(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_list_expr(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_func(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_expr_assign(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);


int sem_var_decl(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_func_decl(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_func_def(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_param_decl(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_scope(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_print(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_do_loop(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_for_loop(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_exit(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);
int sem_continue(SemAnalyzer* sem_an, struct parser_t* parser, SemValue value);

#endif //IFJ17_COMPILER_SEM_ANALYZER_H
