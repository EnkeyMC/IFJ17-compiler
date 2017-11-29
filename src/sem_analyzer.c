/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sem_analyzer.h"
#include "error_code.h"
#include "token.h"
#include "parser.h"
#include "3ac.h"
#include "debug.h"
#include "utils.h"

#define SEM_FSM switch(sem_an->state)
#define SEM_STATE(state) case state:
#define END_STATE break
#define SEM_ERROR_STATE default: return EXIT_INTERN_ERROR
#define SEM_NEXT_STATE(s) sem_an->state = s
#define SEM_SET_EXPR_TYPE(type) sem_an->value = sem_value_init(); \
		if (sem_an->value == NULL) return EXIT_INTERN_ERROR; \
		sem_an->value->value_type = VTYPE_EXPR; \
		sem_an->value->expr_type = type

#define SEM_ACTION_CHECK assert(sem_an != NULL); \
		assert(parser != NULL); \
		if (sem_an->finished) return EXIT_SUCCESS

#define LABEL_PREFIX_ELSE "ELSE_"
#define LABEL_PREFIX_ENDIF "ENDIF_"
#define LABEL_PREFIX_LOOP_COND "LOOP_COND_"
#define LABEL_PREFIX_LOOP_END "LOOP_END_"
#define LABEL_PREFIX_DO_LOOP "DO_LOOP_"


SemAnalyzer* sem_an_init(semantic_action_f sem_action) {
	SemAnalyzer* sem_an = (SemAnalyzer*) malloc(sizeof(SemAnalyzer));
	if (sem_an == NULL)
		return NULL;

	sem_an->sem_action = sem_action;
	sem_an->finished = false;
	sem_an->state = SEM_STATE_START;
	sem_an->value = NULL;

	return sem_an;
}

void sem_an_free(void* sem_an) {
	if (((SemAnalyzer*) sem_an)->value != NULL) {
		sem_value_free(((SemAnalyzer*) sem_an)->value);
	}

	free(sem_an);
}

SemValue* sem_value_init() {
	return (SemValue*) malloc(sizeof(SemValue));
}

SemValue* sem_value_copy(const SemValue* value) {
	if (value == NULL)
		return NULL;

	SemValue* new_val = (SemValue*) malloc(sizeof(SemValue));
	if (new_val == NULL)
		return NULL;

	new_val->value_type = value->value_type;

	switch (value->value_type) {
		case VTYPE_TOKEN:
			new_val->token = token_copy(value->token);
			if (new_val->token == NULL) {
				free(new_val);
				return NULL;
			}
			break;
		case VTYPE_ID:
			new_val->id = value->id;  // We want it to be pointer to symtable so no copy here
			break;
		case VTYPE_LIST:
			// In list we will only move it's content
			new_val->list = dllist_copy(value->list);
			if (new_val->list == NULL)
				return NULL;
			// Clear old list, it won't be needed anymore
			dllist_activate_first(value->list);
			while (dllist_active(value->list)) dllist_delete_and_succ(value->list);
			break;
		case VTYPE_EXPR:
			new_val->expr_type = value->expr_type;
			break;
		case VTYPE_IF:
			new_val->if_val.if_id = (char*) malloc(sizeof(char) * (strlen(value->if_val.if_id) + 1));
			if (new_val->if_val.if_id == NULL) {
				free(new_val);
				return NULL;
			}

			new_val->if_val.elseif_id = (char*) malloc(sizeof(char) * (strlen(value->if_val.elseif_id) + 1));
			if (new_val->if_val.elseif_id == NULL) {
				free(new_val);
				return NULL;
			}
			break;
	}

	return new_val;
}

void sem_value_free(void* value) {
	if (value == NULL)
		return;

	SemValue* to_free = (SemValue*) value;

	switch (to_free->value_type) {
		case VTYPE_TOKEN:
			token_free(to_free->token);
			break;
		case VTYPE_ID:	// Can't free reference to symbol table
			break;
		case VTYPE_LIST:
			dllist_free(to_free->list);
			break;
		case VTYPE_IF:
			if (to_free->if_val.if_id != NULL)
				free(to_free->if_val.if_id);
			if (to_free->if_val.elseif_id != NULL)
				free(to_free->if_val.elseif_id);
		default:
			break;
	}

	free(value);
}

// ------------------------
// SEMANTIC STACK FUNCTIONS
// ------------------------

bool sem_stack_empty(DLList *s) {
	assert(s != NULL);

	return dllist_empty(s);
}

void* sem_stack_top(DLList *s) {
	assert(s != NULL);

	return dllist_get_first(s);
}

void* sem_stack_pop(DLList *s) {
	assert(s != NULL);

	return dllist_delete_first(s);
}

bool sem_stack_push(DLList *s, void* item) {
	assert(s != NULL);

	return dllist_insert_first(s, item);
}

void sem_an_debug(void* sa) {
	SemAnalyzer* sem_an = (SemAnalyzer*) sa;

	debug("SemAnalyzer@%p: {", sem_an);

	if (sem_an != NULL) {
		debug(".sem_action = %p, .finished = %s, .state = %d, .value = ", sem_an->sem_action, sem_an->finished ? "true" : "false", sem_an->state);
		sem_value_debug(sem_an->value);
	}

	debugs("}");
}

void sem_value_debug(void* val) {
	SemValue* value = (SemValue*) val;

	debug("SemValue@%p: {", value);

	if (value != NULL) {
		switch (value->value_type) {
			case VTYPE_TOKEN:
				debugs(".type = TOKEN, .token = ");
				token_debug(value->token);
				break;
			case VTYPE_ID:
				debug(".type = ID, .id = %s", value->id->key);
				break;
			case VTYPE_EXPR:
				debug(".type = EXPR, .expr_type = %d", value->expr_type);
				break;
			case VTYPE_LIST:
				debugs(".type = LIST, .list = ");
				dllist_debug(value->list, sem_value_debug);
				break;
			case VTYPE_IF:
				debug(".type = IF, .if_id = %s, .elseif_id = %s", value->if_val.if_id, value->if_val.elseif_id);
				break;
		}
	}

	debugs("}");
}

// ----------------
// SEMANTIC ACTIONS
// ----------------

static HashTable* get_current_sym_tab(Parser* parser) {
	// Get local value table
	HashTable* symtab = (HashTable*) dllist_get_first(parser->sym_tab_stack);
	if (symtab == NULL) {
		symtab = parser->sym_tab_global;  // If there is no local value table, use global
	}

	return symtab;
}

static const char* get_var_scope_prefix(Parser* parser, const char* key) {
	dllist_activate_first(parser->sym_tab_stack);
	htab_item* item;

	while (dllist_active(parser->sym_tab_stack)) {
		item = htab_find((HashTable*) dllist_get_active(parser->sym_tab_stack), key);
		if (item != NULL)
			return F_LOCAL;

		dllist_succ(parser->sym_tab_stack);
	}

	return F_GLOBAL;
}

static SemAnalyzer* find_sem_action(Parser *parser, semantic_action_f sem_action) {
	dllist_activate_first(parser->sem_an_stack);
	SemAnalyzer *sem_an;

	while (dllist_active(parser->sem_an_stack)) {
		sem_an = (SemAnalyzer*) dllist_get_active(parser->sem_an_stack);
		if (sem_an->sem_action == sem_action)
			return sem_an;

		dllist_succ(parser->sem_an_stack);
	}

	return NULL;
}

static const char* get_current_scope_prefix(Parser* parser) {
	if (dllist_get_first(parser->sym_tab_stack) == NULL) {
		return F_GLOBAL;
	}
	return F_LOCAL;
}

static htab_item* find_symbol(Parser* parser, const char* key) {
	dllist_activate_first(parser->sym_tab_stack);
	htab_item* item;

	while (dllist_active(parser->sym_tab_stack)) {
		item = htab_find((HashTable*) dllist_get_active(parser->sym_tab_stack), key);
		if (item != NULL)
			return item;

		dllist_succ(parser->sym_tab_stack);
	}

	return htab_find(parser->sym_tab_global, key);
}

static HashTable* create_scope(Parser* parser) {
	HashTable* local = htab_init(HTAB_INIT_SIZE);
	if (local == NULL)
		return NULL;

	if (! dllist_insert_first(parser->sym_tab_stack, local)) {
		free(local);
		return NULL;
	}

	return local;
}

static void delete_scope(Parser* parser) {
	if (dllist_empty(parser->sym_tab_stack))
		return;
	else {
		HashTable* local_symtab = (HashTable*) dllist_delete_first(parser->sym_tab_stack);
		htab_free(local_symtab);
	}
}

static bool are_types_compatible(token_e type1, token_e type2) {
	if (type1 == type2) {
		return true;
	} else if (type1 == TOKEN_KW_INTEGER && type2 == TOKEN_KW_DOUBLE) {
		return true;
	} else if (type1 == TOKEN_KW_DOUBLE && type2 == TOKEN_KW_INTEGER) {
		return true;
	}
	return false;
}

static DLList* get_current_il_list(Parser* parser) {
	if (find_sem_action(parser, sem_func_def) != NULL)
		return func_il;
	if (dllist_get_first(parser->sym_tab_stack) != NULL)
		return main_il;
	return global_il;
}

/**
 * Casts second operand with 'inst' instruction
 * @param parser Parser
 * @param inst Instruction to use on second operand (has to be stack instruction)
 * @return true on success, false on internal error
 */
static bool cast_second_operand(Parser* parser, opcode_e inst) {
	// Cast second operand, we need to temporarly pop top operand to access the second one
	char* tmp_var = generate_uid();
	if (tmp_var == NULL)
		return false;

	DLList* il = get_current_il_list(parser);
	// Define tmp_var
	IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	// Pop top of stack to tmp_var
	IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	// Cast second operand
	IL_ADD(il, inst, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	// Push tmp_var back on stack
	IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	free(tmp_var);	// Free generated uid
	return true;
}

// SEMANTIC FUNCTIONS

/**
 * Called at the end of evaluating expression to prepare value for parent SemAnalyzer
 * @param sem_an SemAnalyzer
 * @param parser Parser
 * @param value SemValue
 * @return exit code
 */
int sem_expr_result(SemAnalyzer *sem_an, Parser *parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_EXPR) {
				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				htab_item* item = htab_find(parser->sym_tab_global, EXPR_VALUE_VAR);
				if (item == NULL) {
					IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

					item = htab_lookup(parser->sym_tab_global, EXPR_VALUE_VAR);
					if (item == NULL) {
						return EXIT_INTERN_ERROR;
					}
				}

				item->id_data->type = (token_e) value.expr_type;

				sem_an->value->value_type = VTYPE_ID;
				sem_an->value->id = item;

				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_id(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_TOKEN);
			assert(value.token->id == TOKEN_IDENTIFIER);

			// Check if variable exists
			htab_item* item = find_symbol(parser, value.token->data.str);
			if (item == NULL) {
				return EXIT_SEMANTIC_PROG_ERROR;
			}

			SEM_SET_EXPR_TYPE(item->id_data->type);

			// Push variable on stack
			DLList* il = get_current_il_list(parser);
			IL_ADD(il, OP_PUSHS, addr_symbol(get_var_scope_prefix(parser, item->key), item->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_const(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_TOKEN);

			token_e type = END_OF_TERMINALS;
			switch (value.token->id) {
				case TOKEN_STRING:
					type = TOKEN_KW_STRING;
					break;
				case TOKEN_INT:
					type = TOKEN_KW_INTEGER;
					break;
				case TOKEN_REAL:
					type = TOKEN_KW_DOUBLE;
					break;
				case TOKEN_KW_TRUE:
				case TOKEN_KW_FALSE:
					type = TOKEN_KW_BOOLEAN;
					break;
				default:
					assert(!"I shouldn't be here");
			}

			DLList* il = get_current_il_list(parser);
			IL_ADD(il, OP_PUSHS, addr_constant(*value.token), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

			SEM_SET_EXPR_TYPE(type);
			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_and_or_not(SemAnalyzer *sem_an, Parser *parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_EXPR);

			if (value.expr_type != TOKEN_KW_BOOLEAN) {
				return EXIT_SEMANTIC_COMP_ERROR;
			}

			SEM_NEXT_STATE(SEM_STATE_OPERATOR);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERATOR) {
			assert(value.value_type == VTYPE_TOKEN);

			// Save operator
			sem_an->value = sem_value_copy(&value);
			if (sem_an->value == NULL)
				return EXIT_INTERN_ERROR;

			if (value.token->id == TOKEN_KW_NOT) {
				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_NOTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				SEM_SET_EXPR_TYPE(TOKEN_KW_BOOLEAN);
				sem_an->finished = true;
			} else {
				SEM_NEXT_STATE(SEM_STATE_OPERAND);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERAND) {
			assert(value.value_type == VTYPE_EXPR);

			if (value.expr_type != TOKEN_KW_BOOLEAN) {
				return EXIT_SEMANTIC_COMP_ERROR;
			}

			DLList* il = get_current_il_list(parser);
			switch (sem_an->value->token->id) {
				case TOKEN_KW_OR:
					IL_ADD(il, OP_ORS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_KW_AND:
					IL_ADD(il, OP_ANDS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				default:
					assert(!"I shouldn't be here");
					break;
			}

			SEM_SET_EXPR_TYPE(TOKEN_KW_BOOLEAN);

			sem_an->finished = true;

		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_lte_gte(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	static token_e op_type;  // First operand type

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_EXPR);

			// Remeber operand type
			op_type = (token_e) value.expr_type;

			if (op_type != TOKEN_KW_INTEGER && op_type != TOKEN_KW_DOUBLE && op_type != TOKEN_KW_STRING) {
				return EXIT_SEMANTIC_PROG_ERROR;
			}

			SEM_NEXT_STATE(SEM_STATE_OPERATOR);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERATOR) {
			assert(value.value_type == VTYPE_TOKEN);

			sem_an->value = sem_value_copy(&value);
			if (sem_an->value == NULL)
				return EXIT_INTERN_ERROR;

			SEM_NEXT_STATE(SEM_STATE_OPERAND);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERAND) {
			assert(value.value_type == VTYPE_EXPR);

			token_e type = (token_e) value.expr_type;
			DLList* il = get_current_il_list(parser);

			// Check operand types and implicitly cast if possible
			switch (op_type) {
				case TOKEN_KW_STRING:
					if (type != TOKEN_KW_STRING)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				case TOKEN_KW_INTEGER:
					if (type == TOKEN_KW_DOUBLE) {
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					} else if (type != TOKEN_KW_INTEGER) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}
					break;
				case TOKEN_KW_DOUBLE:
					if (type == TOKEN_KW_INTEGER) {
						// Cast to float
						if (!cast_second_operand(parser, OP_INT2FLOATS))
							return EXIT_INTERN_ERROR;
					} else if (type != TOKEN_KW_DOUBLE) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}
					break;
				default:
					return EXIT_SEMANTIC_COMP_ERROR;
			}

			switch (sem_an->value->token->id) {
				case TOKEN_LT:
					IL_ADD(il, OP_LTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_GT:
					IL_ADD(il, OP_GTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_LE:
					{
						// We have to save operands to tmp variables
						char* tmp1 = generate_uid();
						if (tmp1 == NULL)
							return EXIT_INTERN_ERROR;

						char* tmp2 = generate_uid();
						if (tmp2 == NULL) {
							free(tmp1);
							return EXIT_INTERN_ERROR;
						}

						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// LTS
						IL_ADD(il, OP_LTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// EQS
						IL_ADD(il, OP_EQS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// ORS
						IL_ADD(il, OP_ORS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);


						free(tmp1);
						free(tmp2);
					}
					break;
				case TOKEN_GE:
					{
						// We have to save operands to tmp variables
						char* tmp1 = generate_uid();
						if (tmp1 == NULL)
							return EXIT_INTERN_ERROR;

						char* tmp2 = generate_uid();
						if (tmp2 == NULL) {
							free(tmp1);
							return EXIT_INTERN_ERROR;
						}

						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// GTS
						IL_ADD(il, OP_GTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// EQS
						IL_ADD(il, OP_EQS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// ORS
						IL_ADD(il, OP_ORS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

						free(tmp1);
						free(tmp2);
					}
					break;
				default:
					assert(!"I shouldn't be here");
					break;
			}

			SEM_SET_EXPR_TYPE(TOKEN_KW_BOOLEAN);

			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_eq_ne(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	static token_e op_type;  // First operand type

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_EXPR);

			// Remeber operand type
			op_type = (token_e) value.expr_type;

			if (op_type != TOKEN_KW_INTEGER && op_type != TOKEN_KW_DOUBLE && op_type != TOKEN_KW_STRING && op_type != TOKEN_KW_BOOLEAN) {
				assert(!"I am expecting type...");
				return EXIT_INTERN_ERROR;
			}

			SEM_NEXT_STATE(SEM_STATE_OPERATOR);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERATOR) {
			assert(value.value_type == VTYPE_TOKEN);

			sem_an->value = sem_value_copy(&value);
			if (sem_an->value == NULL)
				return EXIT_INTERN_ERROR;

			SEM_NEXT_STATE(SEM_STATE_OPERAND);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERAND) {
			assert(value.value_type == VTYPE_EXPR);

			token_e type = (token_e) value.expr_type;

			// Check operand types and implicitly cast if possible
			switch (op_type) {
				case TOKEN_KW_STRING:
					if (type != TOKEN_KW_STRING)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				case TOKEN_KW_INTEGER:
					if (type == TOKEN_KW_DOUBLE) {
						DLList* il = get_current_il_list(parser);
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					} else if (type != TOKEN_KW_INTEGER) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}
					break;
				case TOKEN_KW_DOUBLE:
					if (type == TOKEN_KW_INTEGER) {
						if (!cast_second_operand(parser, OP_INT2FLOATS))
							return EXIT_INTERN_ERROR;
					} else if (type != TOKEN_KW_DOUBLE) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}
					break;
				case TOKEN_KW_BOOLEAN:
					if (type != TOKEN_KW_BOOLEAN)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				default:
					return EXIT_SEMANTIC_COMP_ERROR;
			}

			DLList* il = get_current_il_list(parser);
			switch (sem_an->value->token->id) {
				case TOKEN_EQUAL:
					IL_ADD(il, OP_EQS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_NE:
					IL_ADD(il, OP_EQS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					IL_ADD(il, OP_NOTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				default:
					assert(!"I shouldn't be here");
					break;
			}

			SEM_SET_EXPR_TYPE(TOKEN_KW_BOOLEAN);

			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_aritmetic_basic(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	static token_e op_type;  // First operand type

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_EXPR);

			// Remeber operand type
			op_type = (token_e) value.expr_type;

			if (op_type != TOKEN_KW_INTEGER && op_type != TOKEN_KW_DOUBLE && op_type != TOKEN_KW_STRING) {
				return EXIT_SEMANTIC_COMP_ERROR;
			}

			SEM_NEXT_STATE(SEM_STATE_OPERATOR);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERATOR) {
			assert(value.value_type == VTYPE_TOKEN);

			sem_an->value = sem_value_copy(&value);
			if (sem_an->value == NULL)
				return EXIT_INTERN_ERROR;

			SEM_NEXT_STATE(SEM_STATE_OPERAND);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERAND) {
			assert(value.value_type == VTYPE_EXPR);

			token_e type = (token_e) value.expr_type;

			// Check operand types and implicitly cast if possible
			switch (op_type) {
				case TOKEN_KW_STRING:
					if (type != TOKEN_KW_STRING || sem_an->value->token->id != TOKEN_ADD)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				case TOKEN_KW_INTEGER:
					if (type == TOKEN_KW_DOUBLE) {
						DLList* il = get_current_il_list(parser);
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						op_type = TOKEN_KW_INTEGER;
					} else if (type != TOKEN_KW_INTEGER) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}
					break;
				case TOKEN_KW_DOUBLE:
					if (type == TOKEN_KW_INTEGER) {
						if (!cast_second_operand(parser, OP_INT2FLOATS))
							return EXIT_INTERN_ERROR;

						type = TOKEN_KW_DOUBLE;
					} else if (type != TOKEN_KW_DOUBLE) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}
					break;
				default:
					return EXIT_SEMANTIC_COMP_ERROR;
			}

			DLList* il = get_current_il_list(parser);
			switch (sem_an->value->token->id) {
				case TOKEN_ADD:
					if (op_type == TOKEN_KW_STRING) {
						// Cannot concatenate on stack, have to make temp vars
						char* tmp1 = generate_uid();
						if (tmp1 == NULL)
							return EXIT_INTERN_ERROR;

						char* tmp2 = generate_uid();
						if (tmp2 == NULL) {
							free(tmp1);
							return EXIT_INTERN_ERROR;
						}

						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_CONCAT, addr_symbol(F_GLOBAL, tmp1), addr_symbol(F_GLOBAL, tmp2), addr_symbol(F_GLOBAL, tmp1), EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

						free(tmp1);
						free(tmp2);
					} else {
						IL_ADD(il, OP_ADDS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					}
					break;
				case TOKEN_SUB:
					IL_ADD(il, OP_SUBS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_MUL:
					IL_ADD(il, OP_MULS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				default:
					assert(!"I shouldn't be here");
					break;
			}

			SEM_SET_EXPR_TYPE(type);

			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_div(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	static token_e op_type;  // First operand type

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_EXPR);

			// Remeber operand type
			op_type = (token_e) value.expr_type;

			if (op_type != TOKEN_KW_INTEGER && op_type != TOKEN_KW_DOUBLE) {
				return EXIT_SEMANTIC_PROG_ERROR;
			}

			SEM_NEXT_STATE(SEM_STATE_OPERATOR);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERATOR) {
			assert(value.value_type == VTYPE_TOKEN);

			sem_an->value = sem_value_copy(&value);
			if (sem_an->value == NULL)
				return EXIT_INTERN_ERROR;

			SEM_NEXT_STATE(SEM_STATE_OPERAND);
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERAND) {
			assert(value.value_type == VTYPE_EXPR);

			token_e type = (token_e) value.expr_type;

			DLList* il = get_current_il_list(parser);
			// Check operand types and implicitly cast if possible
			switch (sem_an->value->token->id) {
				case TOKEN_DIVI:  // Needs to be casted to flat and then back to int
					if (type == TOKEN_KW_INTEGER) {
						if (!cast_second_operand(parser, OP_INT2FLOATS))
							return EXIT_INTERN_ERROR;
						type = TOKEN_KW_DOUBLE;
					} else if (type == TOKEN_KW_DOUBLE) {
						// Cast second operand, we need to temporarly pop top operand to access the second one
						char* tmp_var = generate_uid();
						IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_FLOAT2R2EINTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						free(tmp_var);	// Free generated string
					} else {
						return EXIT_SEMANTIC_COMP_ERROR;
					}

					if (op_type == TOKEN_KW_DOUBLE) {
						// Round to even
						IL_ADD(il, OP_FLOAT2R2EINTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						op_type = TOKEN_KW_INTEGER;
					} else {  // TOKEN_KW_INTEGER
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					}

					break;
				case TOKEN_DIVR:
					if (type == TOKEN_KW_INTEGER) {
						if (!cast_second_operand(parser, OP_INT2FLOATS))
							return EXIT_INTERN_ERROR;
						type = TOKEN_KW_DOUBLE;
					} else if (type != TOKEN_KW_DOUBLE) {
						return EXIT_SEMANTIC_COMP_ERROR;
					}

					if (op_type == TOKEN_KW_INTEGER) {
						IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					}
					break;
				default:
					return EXIT_SEMANTIC_COMP_ERROR;
			}

			IL_ADD(il, OP_DIVS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

			if (sem_an->value->token->id == TOKEN_DIVI) {
				IL_ADD(il, OP_FLOAT2INTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				type = TOKEN_KW_INTEGER;
			}

			SEM_SET_EXPR_TYPE(type);

			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_brackets(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;
	(void) parser;  // We don't need it
	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_EXPR) {
				// Only copy the value for parent expression
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_unary(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_EXPR) {
				// Check types
				if (value.expr_type != TOKEN_KW_INTEGER && value.expr_type != TOKEN_KW_DOUBLE)
					return EXIT_SEMANTIC_COMP_ERROR;

				// Do 0 - expr
				// Store stack top and push 0 before it
				char* tmp = generate_uid();
				if (tmp == NULL)
					return EXIT_INTERN_ERROR;

				DLList* il = get_current_il_list(parser);
				IL_ADD(global_il, OP_DEFVAR, addr_symbol(F_GLOBAL, tmp), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_POPS, addr_symbol(F_GLOBAL, tmp), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				// Push 0
				if (value.expr_type == TOKEN_KW_INTEGER) {
					IL_ADD(il, OP_PUSHS, addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				} else {
					IL_ADD(il, OP_PUSHS, addr_constant(MAKE_TOKEN_REAL(0)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				}

				// Push tmp back
				IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, tmp), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				// Substract
				IL_ADD(il, OP_SUBS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_list(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;
	(void) parser;  // We don't need it

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_EXPR) {
				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				sem_an->value->value_type = VTYPE_LIST;
				sem_an->value->list = dllist_init(sem_value_free);
				if (sem_an->value->list == NULL)
					return EXIT_INTERN_ERROR;

				SemValue* copy = sem_value_copy(&value);
				if (copy == NULL)
					return EXIT_INTERN_ERROR;
				// Insert backwards
				dllist_insert_first(sem_an->value->list, copy);

				SEM_NEXT_STATE(SEM_STATE_LIST);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_LIST) {
			if (value.value_type == VTYPE_EXPR) {
				SemValue* copy = sem_value_copy(&value);
				if (copy == NULL)
					return EXIT_INTERN_ERROR;
				// Insert backwards
				dllist_insert_first(sem_an->value->list, copy);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_list_expr(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;
	(void) parser;  // We don't need it

	static SemValue* expr;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_EXPR) {
				expr = sem_value_copy(&value);
				if (expr == NULL)
					return EXIT_INTERN_ERROR;

				SEM_NEXT_STATE(SEM_STATE_LIST);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_LIST) {
			if (value.value_type == VTYPE_LIST) {
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				dllist_insert_last(sem_an->value->list, expr);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_func(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_EXPR || value.value_type == VTYPE_LIST) {
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;
			} else if (value.value_type == VTYPE_TOKEN && value.token->id == TOKEN_LPAR) {
				sem_an->value = NULL;  // No parameters
			} else {
				break;
			}
			SEM_NEXT_STATE(SEM_STATE_FUNC_ID);
		} END_STATE;

		SEM_STATE(SEM_STATE_FUNC_ID) {
			if (value.value_type == VTYPE_TOKEN && value.token->id == TOKEN_IDENTIFIER) {
				// Check if function exists
				htab_item* func_item = htab_find(parser->sym_tab_functions, value.token->data.str);
				if (func_item == NULL)
					return EXIT_SEMANTIC_PROG_ERROR;

				// Check parameters
				if (sem_an->value == NULL) {
					// No parameters
					if (func_params_num(func_item) != 0)
						return EXIT_SEMANTIC_COMP_ERROR;
				} else if (sem_an->value->value_type == VTYPE_EXPR) {
					if (func_params_num(func_item) != 1)
						return EXIT_SEMANTIC_COMP_ERROR;

					token_e value_type = (token_e) sem_an->value->expr_type;
					token_e param_type = func_get_param(func_item, 1);
					// 1 Parameter
					if (!are_types_compatible(value_type, param_type))
						return EXIT_SEMANTIC_COMP_ERROR;

				} else {
					// 2 and more params
					dllist_activate_first(sem_an->value->list);
					unsigned i;
					SemValue* expr;
					for (i = 1; dllist_active(sem_an->value->list); i++) {
						expr = (SemValue*) dllist_get_active(sem_an->value->list);
						if (!are_types_compatible((token_e) expr->expr_type, func_get_param(func_item, i)))
							return EXIT_SEMANTIC_COMP_ERROR;
						dllist_succ(sem_an->value->list);
					}

					// If function has more params
					if (func_get_param(func_item, i) != END_OF_TERMINALS)
						return EXIT_SEMANTIC_COMP_ERROR;
				}

				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_CALL, addr_symbol("", func_item->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				sem_value_free(sem_an->value);
				SEM_SET_EXPR_TYPE(func_item->func_data->rt);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_assign(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	static Token* op;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN && value.token->id == TOKEN_IDENTIFIER) {
				htab_item* id = find_symbol(parser, value.token->data.str);
				if (id == NULL)
					return EXIT_SEMANTIC_PROG_ERROR;

				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				sem_an->value->value_type = VTYPE_ID;
				sem_an->value->id = id;

				SEM_NEXT_STATE(SEM_STATE_OPERATOR);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERATOR) {
			if (value.value_type == VTYPE_TOKEN) {
				op = token_copy(value.token);
				if (op == NULL)
					return EXIT_INTERN_ERROR;

				SEM_NEXT_STATE(SEM_STATE_OPERAND);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_OPERAND) {
			if (value.value_type == VTYPE_ID) {
				token_e id_type = sem_an->value->id->id_data->type;
				token_e value_type = (token_e) value.id->id_data->type;
				const char* val_prefix = get_var_scope_prefix(parser, value.id->key);
				const char* prefix = get_current_scope_prefix(parser);

				DLList* il = get_current_il_list(parser);
				switch (op->id) {
					case TOKEN_DIVR_ASIGN:
						if (id_type == TOKEN_KW_INTEGER) {
							IL_ADD(il, OP_INT2FLOAT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
							id_type = TOKEN_KW_DOUBLE;
						}

						if (value_type == TOKEN_KW_INTEGER) {
							IL_ADD(il, OP_INT2FLOAT, addr_symbol(val_prefix, value.id->key), addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
							value_type = TOKEN_KW_DOUBLE;
						}
					case TOKEN_ADD_ASIGN:
						if (id_type == TOKEN_KW_STRING) {
							if (value_type != TOKEN_KW_STRING)
								return EXIT_SEMANTIC_COMP_ERROR;

							IL_ADD(il, OP_CONCAT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), addr_symbol(val_prefix, value.id->key), EXIT_INTERN_ERROR);
							break;
						}
					case TOKEN_SUB_ASIGN:
					case TOKEN_MUL_ASIGN:
						if (id_type == TOKEN_KW_BOOLEAN || id_type == TOKEN_KW_STRING)
							return EXIT_SEMANTIC_COMP_ERROR;
					case TOKEN_EQUAL: {
							if (!are_types_compatible(id_type, value_type))
								return EXIT_SEMANTIC_COMP_ERROR;

							if (value_type == TOKEN_KW_INTEGER && id_type == TOKEN_KW_DOUBLE) {
								IL_ADD(il, OP_INT2FLOAT, addr_symbol(val_prefix, value.id->key), addr_symbol(val_prefix, value.id->key),
									   NO_ADDR, EXIT_INTERN_ERROR);
							} else if (value_type == TOKEN_KW_DOUBLE && id_type == TOKEN_KW_INTEGER) {
								IL_ADD(il, OP_INT2FLOAT, addr_symbol(prefix, sem_an->value->id->key),
									   addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
								id_type = TOKEN_KW_DOUBLE;
							}

							opcode_e assign_operation = OP_MOVE;
							switch (op->id) {
								case TOKEN_SUB_ASIGN:
									assign_operation = OP_SUB;
									break;
								case TOKEN_MUL_ASIGN:
									assign_operation = OP_MUL;
									break;
								case TOKEN_ADD_ASIGN:
									assign_operation = OP_ADD;
									break;
								case TOKEN_DIVR_ASIGN:
									assign_operation = OP_DIV;
								default:
									break;
							}

							if (assign_operation == OP_MOVE) {
								IL_ADD(il, assign_operation, addr_symbol(prefix, sem_an->value->id->key),
									   addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
							} else {
								IL_ADD(il, assign_operation, addr_symbol(prefix, sem_an->value->id->key),
									   addr_symbol(prefix, sem_an->value->id->key), addr_symbol(val_prefix, value.id->key),
									   EXIT_INTERN_ERROR);
							}
							break;
						}
					case TOKEN_DIVI_ASIGN:
						if (id_type == TOKEN_KW_BOOLEAN || id_type == TOKEN_KW_STRING)
							return EXIT_SEMANTIC_COMP_ERROR;

						if (id_type == TOKEN_KW_INTEGER) {
							IL_ADD(il, OP_INT2FLOAT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
						} else if(id_type == TOKEN_KW_DOUBLE) {
							// Round up
							IL_ADD(il, OP_FLOAT2R2EINT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
							IL_ADD(il, OP_INT2FLOAT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
						}

						if (value_type == TOKEN_KW_INTEGER) {
							IL_ADD(il, OP_INT2FLOAT, addr_symbol(val_prefix, value.id->key), addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
						} else if(value_type == TOKEN_KW_DOUBLE) {
							// Round up
							IL_ADD(il, OP_FLOAT2R2EINT, addr_symbol(val_prefix, value.id->key), addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
							IL_ADD(il, OP_INT2FLOAT, addr_symbol(val_prefix, value.id->key), addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
						}

						IL_ADD(il, OP_DIV, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), addr_symbol(val_prefix, value.id->key), EXIT_INTERN_ERROR);
						IL_ADD(il, OP_FLOAT2INT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
						id_type = TOKEN_KW_INTEGER;
						break;
					default:
						assert(!"I shouldn't be here");
				}

				if (id_type == TOKEN_KW_DOUBLE && sem_an->value->id->id_data->type == TOKEN_KW_INTEGER) {
					IL_ADD(il, OP_FLOAT2R2EINT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
				} else if (id_type == TOKEN_KW_INTEGER && sem_an->value->id->id_data->type == TOKEN_KW_DOUBLE) {
					IL_ADD(il, OP_INT2FLOAT, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, sem_an->value->id->key), NO_ADDR, EXIT_INTERN_ERROR);
				}

				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_var_decl(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	HashTable* symtab = NULL;
	HashTable* symtab_func = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER)
			{
				symtab = get_current_sym_tab(parser);
				symtab_func = parser->sym_tab_functions;

				// Check variable redefinition
				if (htab_find(symtab, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}
				if (htab_find(symtab_func, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put variable in value table
				htab_item* item = htab_lookup(symtab, value.token->data.str);
				if (item == NULL) {
					return EXIT_INTERN_ERROR;
				}

				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_DEFVAR, addr_symbol(get_current_scope_prefix(parser), value.token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_VAR_TYPE);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_VAR_TYPE) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN: {
						sem_an->value->token->id = value.token->id;

						SEM_NEXT_STATE(SEM_STATE_ASSIGN);
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_ASSIGN) {
			DLList* il = get_current_il_list(parser);

			if (value.value_type == VTYPE_ID) {  // Variable initialization
				const char* prefix = get_current_scope_prefix(parser);
				const char* val_prefix = get_var_scope_prefix(parser, value.id->key);
				token_e value_type = (token_e) value.id->id_data->type;
				token_e id_type = sem_an->value->token->id;

				if (value_type == TOKEN_KW_INTEGER && id_type == TOKEN_KW_DOUBLE) {
					IL_ADD(il, OP_INT2FLOAT, addr_symbol(prefix, sem_an->value->token->data.str), addr_symbol(val_prefix, value.id->key),
						   NO_ADDR, EXIT_INTERN_ERROR);
				} else if (value_type == TOKEN_KW_DOUBLE && id_type == TOKEN_KW_INTEGER) {
					IL_ADD(il, OP_FLOAT2R2EINT, addr_symbol(prefix, sem_an->value->token->data.str),
						   addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
				} else if (are_types_compatible(value_type, id_type)) {
					IL_ADD(il, OP_MOVE, addr_symbol(prefix, sem_an->value->token->data.str), addr_symbol(val_prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
				} else {
					return EXIT_SEMANTIC_COMP_ERROR;
				}

				symtab = get_current_sym_tab(parser);
				htab_item* item = htab_lookup(symtab, sem_an->value->token->data.str);
				if (item == NULL)
					return EXIT_INTERN_ERROR;
				item->id_data->type = sem_an->value->token->id;
				sem_an->value->token->id = TOKEN_IDENTIFIER;

				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				sem_an->finished = true;
			} else if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {	// Default initialization
					case TOKEN_EOL: {
						switch (sem_an->value->token->id)
						{
							case TOKEN_KW_INTEGER:
								IL_ADD(il, OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->token->data.str), addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							case TOKEN_KW_BOOLEAN:
								IL_ADD(il, OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->token->data.str), addr_constant(MAKE_TOKEN_BOOL(false)), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							case TOKEN_KW_DOUBLE:
								IL_ADD(il, OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->token->data.str), addr_constant(MAKE_TOKEN_REAL(0)), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							case TOKEN_KW_STRING:
								IL_ADD(il, OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->token->data.str), addr_constant(MAKE_TOKEN_STRING("")), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							default:
								assert(!"I shouldn't be here");
						}
						symtab = get_current_sym_tab(parser);
						htab_item* item = htab_lookup(symtab, sem_an->value->token->data.str);
						if (item == NULL)
							return EXIT_INTERN_ERROR;
						item->id_data->type = sem_an->value->token->id;
						sem_an->value->token->id = TOKEN_IDENTIFIER;
						IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
						sem_an->finished = true;
						break;
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_param_decl(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	HashTable* symtab_func = NULL;
	HashTable* symtab = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER) {
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				symtab_func = parser->sym_tab_functions;
				symtab = get_current_sym_tab(parser);

				// Check variable redefinition
				if (htab_find(symtab, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}
				// Check collision with function name
				if (htab_find(symtab_func, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put variable in local symtable
				if (htab_lookup(symtab, value.token->data.str) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				SEM_NEXT_STATE(SEM_STATE_VAR_TYPE);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_VAR_TYPE) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN: {
						symtab = get_current_sym_tab(parser);
						htab_item *item = htab_find(symtab, sem_an->value->token->data.str);
						item->id_data->type = value.token->id;

						token_free(sem_an->value->token);
						sem_an->value->value_type = VTYPE_ID;
						sem_an->value->id = item;
						sem_an->finished = true;
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_func_decl(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	HashTable* symtab_func = NULL;
	HashTable* symtab_global  = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER) {
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				// Get symtable storing functions
				symtab_func = parser->sym_tab_functions;
				symtab_global = parser->sym_tab_global;

				// Check function redefinition/redeclaration
				if (htab_find(symtab_func, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}
				if (htab_find(symtab_global, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put function name in symtable
				if (htab_func_lookup(symtab_func, value.token->data.str) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				// Create new local symtable
				if (create_scope(parser) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				SEM_NEXT_STATE(SEM_STATE_VAR_TYPE);
			}
		} END_STATE;

		// Determine parameter type
		SEM_STATE(SEM_STATE_VAR_TYPE) {
			if (value.value_type == VTYPE_ID) {
				switch (value.id->id_data->type) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN: {
						symtab_func = parser->sym_tab_functions;
						htab_item *item = htab_find(symtab_func, sem_an->value->token->data.str);
						if (!func_add_param(item, value.id->id_data->type))
							return EXIT_INTERN_ERROR;
					}
					default:
						break;
				};
			}
				// End of parametr declarations
			else if (value.value_type == VTYPE_TOKEN
					 && value.token->id == TOKEN_RPAR) {
				SEM_NEXT_STATE(SEM_STATE_FUNC_RETURN_TYPE);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FUNC_RETURN_TYPE) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN:
					{
						symtab_func = parser->sym_tab_functions;
						htab_item* item = htab_find(symtab_func, sem_an->value->token->data.str);
						func_set_rt(item, value.token->id);

						SEM_NEXT_STATE(SEM_STATE_EOL);
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_EOL) {
			if (value.value_type == VTYPE_TOKEN) {
				if (value.token->id == TOKEN_EOL) {
					delete_scope(parser);
					sem_an->finished = true;
				}
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_scope(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_SCOPE)
			{
				// main scope statement encountered
				if ((find_sem_action(parser, sem_func_def) == NULL)
						&& (dllist_get_first(parser->sym_tab_stack) == NULL)) {
					// check that every function have been defined
					if (!htab_check_definition(parser->sym_tab_functions))
						return EXIT_SEMANTIC_PROG_ERROR;
				}

				if (create_scope(parser) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_SCOPE_END);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_SCOPE_END) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_SCOPE)
			{
				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				delete_scope(parser);
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_print(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_ID) {
				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_WRITE, addr_symbol(F_GLOBAL, value.id->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
			} else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_input(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	htab_item* item = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
					&& value.token->id == TOKEN_IDENTIFIER) {

				item = find_symbol(parser, value.token->data.str);
				if (item == NULL)
					return EXIT_SEMANTIC_PROG_ERROR;
				DLList* il = get_current_il_list(parser);

				IL_ADD(il, OP_WRITE, addr_constant(MAKE_TOKEN_STRING("?\\032")), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				switch (item->id_data->type) {
					case TOKEN_KW_INTEGER:
				IL_ADD(il, OP_READ, addr_symbol(F_LOCAL, value.token->data.str), addr_symbol("int",""), NO_ADDR, EXIT_INTERN_ERROR);
						break;
					case TOKEN_KW_BOOLEAN:
				IL_ADD(il, OP_READ, addr_symbol(F_LOCAL, value.token->data.str), addr_symbol("bool",""), NO_ADDR, EXIT_INTERN_ERROR);
						break;
					case TOKEN_KW_DOUBLE:
				IL_ADD(il, OP_READ, addr_symbol(F_LOCAL, value.token->data.str), addr_symbol("float",""), NO_ADDR, EXIT_INTERN_ERROR);
						break;
					case TOKEN_KW_STRING:
				IL_ADD(il, OP_READ, addr_symbol(F_LOCAL, value.token->data.str), addr_symbol("string",""), NO_ADDR, EXIT_INTERN_ERROR);
						break;
					default:
						break;
				}
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
			} else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_func_def(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;
	HashTable* symtab_func = NULL;
	HashTable* symtab_global  = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER) {

				// Get symtable storing functions
				symtab_func = parser->sym_tab_functions;
				symtab_global = parser->sym_tab_global;

				// Check collision with global variable
				if (htab_find(symtab_global, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}
				// Function can be already declared, but it must not be defined
				htab_item *item = htab_find(symtab_func, value.token->data.str);
				if (item != NULL) {
					if (item->func_data->definition == true) // Already defined
						return EXIT_SEMANTIC_PROG_ERROR;
					else {
						sem_an->value = sem_value_init();
						if (sem_an->value == NULL)
							return EXIT_INTERN_ERROR;

						sem_an->value->value_type = VTYPE_ID;
						sem_an->value->id = item;

						// Create new local symtable
						if (create_scope(parser) == NULL)
							return EXIT_INTERN_ERROR;

						IL_ADD_SPACE(func_il, EXIT_INTERN_ERROR);
						IL_ADD_SPACE(func_il, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_LABEL, addr_symbol("", item->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

						// Function was declared
						SEM_NEXT_STATE(SEM_STATE_DECLARED_VAR_TYPE);
						break;
					}
				}

				// Function was NOT declared
				// add it to symtable
				item = htab_func_lookup(symtab_func, value.token->data.str);
				if (item == NULL) {
					return EXIT_INTERN_ERROR;
				}

				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				sem_an->value->value_type = VTYPE_ID;
				sem_an->value->id = item;

				// Create new local symtable
				if (create_scope(parser) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				IL_ADD_SPACE(func_il, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(func_il, EXIT_INTERN_ERROR);
				IL_ADD(func_il, OP_LABEL, addr_symbol("", value.token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(func_il, OP_CREATEFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(func_il, OP_PUSHFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_VAR_TYPE);
			}
		} END_STATE;

		// Check parameters with function declaration
		SEM_STATE(SEM_STATE_DECLARED_VAR_TYPE) {
			unsigned idx = func_get_param_idx(sem_an->value->id);
			if (value.value_type == VTYPE_ID) {
				switch (value.id->id_data->type) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN: {
						// Type in declaration and definition does not match
						if (value.id->id_data->type != func_get_param(sem_an->value->id, idx))
							return EXIT_SEMANTIC_PROG_ERROR;
						if (!func_store_param_name(sem_an->value->id, value.id->key))
							return EXIT_INTERN_ERROR;
					}
					default:
						break;
				};
			}
				// End of parametr declarations
			else if (value.value_type == VTYPE_TOKEN
					 && value.token->id == TOKEN_RPAR) {
				// Function definition did not provide enough parameters
				if (sem_an->value->id->func_data->par_num != idx -1)
					return EXIT_SEMANTIC_PROG_ERROR;

				// Define parameters in local scope
				for (unsigned int i = func_params_num(sem_an->value->id); i > 0; --i) {
					IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					IL_ADD(func_il, OP_POPS, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					// Implicitly cast parameters
					if (func_get_param(sem_an->value->id, i) == TOKEN_KW_DOUBLE) {
						char* label = generate_uid();
						if (label == NULL)
							return EXIT_INTERN_ERROR;

						IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", label), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("int")), EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_INT2FLOAT, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_LABEL, addr_symbol("", label), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						free(label);
					} else if (func_get_param(sem_an->value->id, i) == TOKEN_KW_INTEGER) {
						char* label = generate_uid();
						if (label == NULL)
							return EXIT_INTERN_ERROR;

						IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", label), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")), EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_LABEL, addr_symbol("", label), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						free(label);
					}
				}

				SEM_NEXT_STATE(SEM_STATE_DECLARED_RETURN_TYPE);
			}
		} END_STATE;

		// Determine parameter type
		SEM_STATE(SEM_STATE_VAR_TYPE) {
			if (value.value_type == VTYPE_ID) {
				switch (value.id->id_data->type) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN: {
						if (!func_store_param_name(sem_an->value->id, value.id->key))
							return EXIT_INTERN_ERROR;
						if (!func_add_param(sem_an->value->id, value.id->id_data->type))
							return EXIT_INTERN_ERROR;
					}
					default:
						break;
				};
			}
				// End of parametr declarations
			else if (value.value_type == VTYPE_TOKEN
					 && value.token->id == TOKEN_RPAR) {

				// Define parameters in local scope
				for (unsigned int i = func_params_num(sem_an->value->id); i > 0; --i) {
					IL_ADD(func_il, OP_DEFVAR, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					IL_ADD(func_il, OP_POPS, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					// Implicitly cast parameters
					if (func_get_param(sem_an->value->id, i) == TOKEN_KW_DOUBLE) {
						char* label = generate_uid();
						if (label == NULL)
							return EXIT_INTERN_ERROR;

						IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", label), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("int")), EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_INT2FLOAT, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_LABEL, addr_symbol("", label), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						free(label);
					} else if (func_get_param(sem_an->value->id, i) == TOKEN_KW_INTEGER) {
						char* label = generate_uid();
						if (label == NULL)
							return EXIT_INTERN_ERROR;

						IL_ADD(func_il, OP_TYPE, addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_JUMPIFNEQ, addr_symbol("", label), addr_symbol(F_GLOBAL, EXPR_VALUE_VAR), addr_constant(MAKE_TOKEN_STRING("float")), EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_FLOAT2R2EINT, addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), addr_symbol(F_LOCAL, func_get_param_name(sem_an->value->id, i)), NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(func_il, OP_LABEL, addr_symbol("", label), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						free(label);
					}
				}

				SEM_NEXT_STATE(SEM_STATE_FUNC_RETURN_TYPE);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DECLARED_RETURN_TYPE) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN:
					{
						if (sem_an->value->id->func_data->rt != value.token->id)
							return EXIT_SEMANTIC_PROG_ERROR;
						func_set_def(sem_an->value->id);

						SEM_NEXT_STATE(SEM_STATE_FUNC_END);
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FUNC_RETURN_TYPE) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN:
					{
						func_set_rt(sem_an->value->id, value.token->id);
						func_set_def(sem_an->value->id);

						SEM_NEXT_STATE(SEM_STATE_FUNC_END);
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FUNC_END) {
			if (value.value_type == VTYPE_TOKEN) {
				if (value.token->id == TOKEN_KW_FUNCTION) {
					delete_scope(parser);
					// Push default return value on stack
					switch (func_get_rt(sem_an->value->id)) {
						case TOKEN_KW_INTEGER:
							IL_ADD(func_il, OP_PUSHS, addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
							break;
						case TOKEN_KW_DOUBLE:
							IL_ADD(func_il, OP_PUSHS, addr_constant(MAKE_TOKEN_REAL(0)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
							break;
						case TOKEN_KW_STRING:
							IL_ADD(func_il, OP_PUSHS, addr_constant(MAKE_TOKEN_STRING("")), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
							break;
						case TOKEN_KW_BOOLEAN:
							IL_ADD(func_il, OP_PUSHS, addr_constant(MAKE_TOKEN_BOOL(false)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
							break;
						default:
							break;
					}
					IL_ADD(func_il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					// Return in case function doesn't end with return
					IL_ADD(func_il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					sem_an->finished = true;
				}
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_do_loop(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	DLList* il = get_current_il_list(parser);

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_DO)
			{
				if (create_scope(parser) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				// Generate id for condition
				char* id = generate_uid();
				if (id == NULL) {
					free(sem_an->value);
					return EXIT_INTERN_ERROR;
				}

				// Save id to SemValue
				sem_an->value->value_type = VTYPE_TOKEN;
				Token token = token_make_str(id);
				free(id);
				sem_an->value->token = token_copy(&token);
				if (sem_an->value->token == NULL) {
					free(sem_an->value);
					return EXIT_INTERN_ERROR;
				}
				sem_an->value->token->id = TOKEN_IDENTIFIER;

				// Create label
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_DO_LOOP, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_DO_TEST_TYPE);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_TEST_TYPE) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_KW_WHILE)
			{
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_COND, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_DO_WHILE);
			}
			else if (value.value_type == VTYPE_TOKEN
					&& value.token->id == TOKEN_KW_UNTIL)
			{
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_COND, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_DO_UNTIL);
			}
			else
			{
				SEM_NEXT_STATE(SEM_STATE_DO_LOOP_TEST_END);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_WHILE) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type != TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;

				const char* prefix = get_var_scope_prefix(parser, value.id->key);
				IL_ADD(il, OP_JUMPIFEQ,
					   addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str),
					   addr_symbol(prefix, value.id->key),
					   addr_constant(MAKE_TOKEN_BOOL(false)),
					   EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_DO_LOOP_TEST_START);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_UNTIL) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type != TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;
				const char* prefix = get_var_scope_prefix(parser, value.id->key);
				IL_ADD(il, OP_JUMPIFEQ,
					   addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str),
					   addr_symbol(prefix, value.id->key),
					   addr_constant(MAKE_TOKEN_BOOL(true)),
					   EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_DO_LOOP_TEST_START);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_LOOP_TEST_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_KW_LOOP)
			{
				// Jump to condition
				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_LOOP_COND, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				// End of loop
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				delete_scope(parser);
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_LOOP_TEST_END) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_KW_LOOP)
			{
				delete_scope(parser);
				SEM_NEXT_STATE(SEM_STATE_DO_TEST_TYPE_END);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_TEST_TYPE_END) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_KW_WHILE)
			{
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_COND, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_DO_WHILE_END);
			}
			else if (value.value_type == VTYPE_TOKEN
					&& value.token->id == TOKEN_KW_UNTIL)
			{
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_COND, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				SEM_NEXT_STATE(SEM_STATE_DO_UNTIL_END);
			}
			else
			{
				// Infinite loop
				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_DO_LOOP, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_UNTIL_END) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type != TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;
				const char* prefix = get_var_scope_prefix(parser, value.id->key);
				IL_ADD(il, OP_JUMPIFEQ,
					   addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str),
					   addr_symbol(prefix, value.id->key),
					   addr_constant(MAKE_TOKEN_BOOL(true)),
					   EXIT_INTERN_ERROR);
				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_DO_LOOP, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_WHILE_END) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type != TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;
				const char* prefix = get_var_scope_prefix(parser, value.id->key);
				IL_ADD(il, OP_JUMPIFEQ,
					   addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str),
					   addr_symbol(prefix, value.id->key),
					   addr_constant(MAKE_TOKEN_BOOL(false)),
					   EXIT_INTERN_ERROR);
				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_DO_LOOP, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_LOOP_END, sem_an->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_for_loop(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	HashTable* symtab = NULL;
	htab_item* item = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER)
			{
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;
				SEM_NEXT_STATE(SEM_STATE_FOR_ITERATOR);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FOR_ITERATOR) {
			if (value.value_type == VTYPE_TOKEN
					 && value.token->id == TOKEN_EQUAL) {

				// variable must have been declared before
				item = find_symbol(parser, sem_an->value->token->data.str);
				if (item == NULL)
					return EXIT_SEMANTIC_PROG_ERROR;
				// variable must be integer or double
				if (item->id_data->type == TOKEN_KW_STRING
						|| item->id_data->type == TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;

				if (create_scope(parser) == NULL)
					return EXIT_INTERN_ERROR;

				SEM_NEXT_STATE(SEM_STATE_FOR_INIT);
			}

			else if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN:
						return EXIT_SEMANTIC_COMP_ERROR; // bad iterator type
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE: {

						if (create_scope(parser) == NULL)
							return EXIT_INTERN_ERROR;

						// add iterator variable to new scope
						symtab = get_current_sym_tab(parser);
						item = htab_lookup(symtab, sem_an->value->token->data.str);
						if (item == NULL) {
							return EXIT_INTERN_ERROR;
						}
						item->id_data->type = value.token->id;

						SEM_NEXT_STATE(SEM_STATE_FOR_INIT);
					}
					default:
						break;
				};
			}
		} END_STATE;

		// Check iterator expression init type
		SEM_STATE(SEM_STATE_FOR_INIT) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type == TOKEN_KW_STRING)
					return EXIT_SEMANTIC_COMP_ERROR;

				SEM_NEXT_STATE(SEM_STATE_FOR_ENDVAL);
			}
		} END_STATE;

		// Check iterator expression end value type
		SEM_STATE(SEM_STATE_FOR_ENDVAL) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type == TOKEN_KW_STRING)
					return EXIT_SEMANTIC_COMP_ERROR;

				SEM_NEXT_STATE(SEM_STATE_FOR_STEP);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FOR_STEP) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_NEXT)
			{
				SEM_NEXT_STATE(SEM_STATE_FOR_END);
			}
			else if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type == TOKEN_KW_STRING)
					return EXIT_SEMANTIC_COMP_ERROR;

				SEM_NEXT_STATE(SEM_STATE_FOR_NEXT);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FOR_NEXT) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_NEXT)
			{
				SEM_NEXT_STATE(SEM_STATE_FOR_END);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_FOR_END) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				delete_scope(parser);
				sem_an->finished = true;
			}
			else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_IDENTIFIER)
			{
				item = find_symbol(parser, sem_an->value->token->data.str);
				if (strcmp(item->key, value.token->data.str) != 0)
					return EXIT_SEMANTIC_PROG_ERROR;
				SEM_NEXT_STATE(SEM_STATE_EOL);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_EOL) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				delete_scope(parser);
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_exit(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SemAnalyzer* sem_action = NULL;
	DLList* il = get_current_il_list(parser);

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_DO)
			{
				sem_action = find_sem_action(parser, sem_do_loop);
				if (sem_action == NULL)
					return EXIT_SEMANTIC_OTHER_ERROR;

				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_LOOP_END, sem_action->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_EOL);
				break;
			}
			else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_FOR)
			{
				sem_action = find_sem_action(parser, sem_for_loop);
				if (sem_action == NULL)
					return EXIT_SEMANTIC_OTHER_ERROR;

				SEM_NEXT_STATE(SEM_STATE_EOL);
				break;
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_EOL) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_continue(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;
	// TODO add instructions
	SemAnalyzer* sem_action = NULL;
	DLList* il = get_current_il_list(parser);

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_DO)
			{
				sem_action = find_sem_action(parser, sem_do_loop);
				if (sem_action == NULL)
					return EXIT_SEMANTIC_OTHER_ERROR;

				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_LOOP_COND, sem_action->value->token->data.str), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_EOL);
				break;
			}
			else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_FOR)
			{
				sem_action = find_sem_action(parser, sem_for_loop);
				if (sem_action == NULL)
					return EXIT_SEMANTIC_OTHER_ERROR;

				SEM_NEXT_STATE(SEM_STATE_EOL);
				break;
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_EOL) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_return(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SemAnalyzer* sem_action = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_ID)
			{
				sem_action = find_sem_action(parser, sem_func_def);
				if (sem_action == NULL)
					return EXIT_SYNTAX_ERROR;

				token_e ret_type = func_get_rt(sem_action->value->id);
				token_e id_type = value.id->id_data->type;
				if (!are_types_compatible(ret_type, id_type))
					return EXIT_SEMANTIC_COMP_ERROR;

				DLList* il = get_current_il_list(parser);
				// Push return value on stack
				IL_ADD(il, OP_PUSHS, addr_symbol(F_GLOBAL, value.id->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				// Cast it to right type if needed
				if (ret_type == TOKEN_KW_INTEGER && id_type == TOKEN_KW_DOUBLE) {
					IL_ADD(il, OP_FLOAT2R2EINTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				} else if (ret_type == TOKEN_KW_DOUBLE && id_type == TOKEN_KW_INTEGER) {
					IL_ADD(il, OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				}
				// Pop local frame
				IL_ADD(il, OP_POPFRAME, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				// Return from function
				IL_ADD(il, OP_RETURN, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_condition(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;
	// Conditions have id saved in Token as string with type TOKEN_IDENTIFIER

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type != TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;

				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				// Generate id for if
				char* id = generate_uid();
				if (id == NULL) {
					free(sem_an->value);
					return EXIT_INTERN_ERROR;
				}

				// Generate id for elseif
				char* elseif_id = generate_uid();
				if (elseif_id == NULL) {
					free(sem_an->value);
					return EXIT_INTERN_ERROR;
				}

				// Save id to SemValue
				sem_an->value->value_type = VTYPE_IF;
				sem_an->value->if_val.if_id = id;
				sem_an->value->if_val.elseif_id = elseif_id;

				DLList* il = get_current_il_list(parser);
				const char* prefix = get_var_scope_prefix(parser, value.id->key);
				// If condition is false, jump to else (might be else if)
				IL_ADD(il, OP_JUMPIFEQ,
					   addr_symbol(LABEL_PREFIX_ELSE, sem_an->value->if_val.elseif_id),
					   addr_symbol(prefix, value.id->key),
					   addr_constant(MAKE_TOKEN_BOOL(false)),
					   EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_IF_EOL);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_IF_EOL) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				if (create_scope(parser) == NULL)
					return EXIT_INTERN_ERROR;

				SEM_NEXT_STATE(SEM_STATE_IF_CONT);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_IF_CONT) {
			DLList* il = get_current_il_list(parser);
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_END)
			{
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_ENDIF, sem_an->value->if_val.if_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_ELSE, sem_an->value->if_val.elseif_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				delete_scope(parser);
				sem_value_free(sem_an->value);
				sem_an->value = NULL;
				sem_an->finished = true;
			}
			else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_ELSE)
			{
				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_ENDIF, sem_an->value->if_val.if_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_ELSE, sem_an->value->if_val.elseif_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				delete_scope(parser);
				if (create_scope(parser) == NULL)
					return EXIT_INTERN_ERROR;
				SEM_NEXT_STATE(SEM_STATE_IF_ELSE);
			}
			else if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_ELSEIF)
			{
				IL_ADD(il, OP_JUMP, addr_symbol(LABEL_PREFIX_ENDIF, sem_an->value->if_val.if_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_ELSE, sem_an->value->if_val.elseif_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				delete_scope(parser);
				SEM_NEXT_STATE(SEM_STATE_IF_ELSEIF_COND);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_IF_ELSEIF_COND) {
			if (value.value_type == VTYPE_ID)
			{
				if (value.id->id_data->type != TOKEN_KW_BOOLEAN)
					return EXIT_SEMANTIC_COMP_ERROR;

				// Generate new id for elseif
				char* id = generate_uid();
				if (id == NULL) {
					free(sem_an->value);
					return EXIT_INTERN_ERROR;
				}

				free(sem_an->value->if_val.elseif_id);  // Free old ID
				sem_an->value->if_val.elseif_id = id;  // Assign new one

				DLList* il = get_current_il_list(parser);
				const char* prefix = get_var_scope_prefix(parser, value.id->key);
				// If condition is false, jump to else (might be else if)
				IL_ADD(il, OP_JUMPIFEQ,
					   addr_symbol(LABEL_PREFIX_ELSE, sem_an->value->if_val.elseif_id),
					   addr_symbol(prefix, value.id->key),
					   addr_constant(MAKE_TOKEN_BOOL(false)),
					   EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);

				SEM_NEXT_STATE(SEM_STATE_IF_ELSEIF_EOL);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_IF_ELSEIF_EOL) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				if (create_scope(parser) == NULL)
					return EXIT_INTERN_ERROR;

				SEM_NEXT_STATE(SEM_STATE_IF_CONT);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_IF_ELSE) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_END)
			{
				DLList* il = get_current_il_list(parser);
				IL_ADD(il, OP_LABEL, addr_symbol(LABEL_PREFIX_ENDIF, sem_an->value->if_val.if_id), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD_SPACE(il, EXIT_INTERN_ERROR);
				delete_scope(parser);
				sem_value_free(sem_an->value);
				sem_an->value = NULL;
				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}
