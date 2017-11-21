#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "sem_analyzer.h"
#include "error_code.h"
#include "token.h"
#include "parser.h"
#include "3ac.h"

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
	}

	return new_val;
}

void sem_value_free(void* value) {
	SemValue* to_free = (SemValue*) value;

	switch (to_free->value_type) {
		case VTYPE_TOKEN:
			token_free(to_free->token);
			break;
		case VTYPE_ID:  // Can't free reference to symbol table
			break;
		case VTYPE_LIST:
			dllist_free(to_free->list);
			break;
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

	// Get tmp_var prefix
	const char* prefix = get_current_scope_prefix(parser);
	// Define tmp_var
	IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	// Pop top of stack to tmp_var
	IL_ADD(OP_POPS, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	// Cast second operand
	IL_ADD(inst, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	// Push tmp_var back on stack
	IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
	free(tmp_var);  // Free generated uid
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

				char* tmp_var = generate_uid();
				if (tmp_var == NULL)
					return EXIT_INTERN_ERROR;

				const char* prefix = get_current_scope_prefix(parser);

				htab_item* item = htab_lookup(get_current_sym_tab(parser), tmp_var);
				if (item == NULL) {
					free(tmp_var);
					return EXIT_INTERN_ERROR;
				}

				item->id_data->type = (token_e) value.expr_type;

				sem_an->value->value_type = VTYPE_ID;
				sem_an->value->id = item;

				IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				IL_ADD(OP_POPS, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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
			IL_ADD(OP_PUSHS, addr_symbol(get_var_scope_prefix(parser, item->key), item->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

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

			IL_ADD(OP_PUSHS, addr_constant(*value.token), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

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
				IL_ADD(OP_NOTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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

			switch (sem_an->value->token->id) {
				case TOKEN_KW_OR:
					IL_ADD(OP_ORS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_KW_AND:
					IL_ADD(OP_ANDS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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

			// Check operand types and implicitly cast if possible
			switch (op_type) {
				case TOKEN_KW_STRING:
					if (type != TOKEN_KW_STRING)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				case TOKEN_KW_INTEGER:
					if (type == TOKEN_KW_DOUBLE) {
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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
					IL_ADD(OP_LTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_GT:
					IL_ADD(OP_GTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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

						const char* prefix = get_current_scope_prefix(parser);

						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// Swap operands
						IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// Negate condition
						IL_ADD(OP_GTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

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

						const char* prefix = get_current_scope_prefix(parser);

						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// Swap operands
						IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						// Negate condition
						IL_ADD(OP_LTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

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

			// Check operand types and implicitly cast if possible
			switch (op_type) {
				case TOKEN_KW_STRING:
					if (type != TOKEN_KW_STRING)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				case TOKEN_KW_INTEGER:
					if (type == TOKEN_KW_DOUBLE) {
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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
				default:
					return EXIT_SEMANTIC_COMP_ERROR;
			}

			switch (sem_an->value->token->id) {
				case TOKEN_EQUAL:
					IL_ADD(OP_EQS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_NE:
					IL_ADD(OP_EQS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					IL_ADD(OP_NOTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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

			// Check operand types and implicitly cast if possible
			switch (op_type) {
				case TOKEN_KW_STRING:
					if (type != TOKEN_KW_STRING && sem_an->value->token->id != TOKEN_ADD)
						return EXIT_SEMANTIC_COMP_ERROR;
					break;
				case TOKEN_KW_INTEGER:
					if (type == TOKEN_KW_DOUBLE) {
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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

						const char* prefix = get_current_scope_prefix(parser);

						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp2), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_CONCAT, addr_symbol(prefix, tmp1), addr_symbol(prefix, tmp2), addr_symbol(prefix, tmp1), EXIT_INTERN_ERROR);
						IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp1), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

						free(tmp1);
						free(tmp2);
					} else {
						IL_ADD(OP_ADDS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					}
					break;
				case TOKEN_SUB:
					IL_ADD(OP_SUBS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					break;
				case TOKEN_MUL:
					IL_ADD(OP_MULS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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
						const char* prefix = get_current_scope_prefix(parser);
						IL_ADD(OP_DEFVAR, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_POPS, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_FLOAT2R2EINTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp_var), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						free(tmp_var);  // Free generated string
					} else {
						return EXIT_SEMANTIC_COMP_ERROR;
					}

					if (op_type == TOKEN_KW_DOUBLE) {
						// Round to even
						IL_ADD(OP_FLOAT2R2EINTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
						op_type = TOKEN_KW_INTEGER;
					} else {  // TOKEN_KW_INTEGER
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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
						IL_ADD(OP_INT2FLOATS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
					}
					break;
				default:
					return EXIT_SEMANTIC_COMP_ERROR;
			}

			IL_ADD(OP_DIVS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

			if (sem_an->value->token->id == TOKEN_DIVI) {
				IL_ADD(OP_FLOAT2R2EINTS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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

				const char* prefix = get_current_scope_prefix(parser);
				IL_ADD(OP_POPS, addr_symbol(prefix, tmp), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				// Push 0
				if (value.expr_type == TOKEN_KW_INTEGER) {
					IL_ADD(OP_PUSHS, addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				} else {
					IL_ADD(OP_PUSHS, addr_constant(MAKE_TOKEN_REAL(0)), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
				}

				// Push tmp back
				IL_ADD(OP_PUSHS, addr_symbol(prefix, tmp), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				// Substract
				IL_ADD(OP_SUBS, NO_ADDR, NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

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

				dllist_insert_last(sem_an->value->list, copy);

				SEM_NEXT_STATE(SEM_STATE_LIST);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_LIST) {
			if (value.value_type == VTYPE_EXPR) {
				SemValue* copy = sem_value_copy(&value);
				if (copy == NULL)
					return EXIT_INTERN_ERROR;

				dllist_insert_last(sem_an->value->list, copy);

				sem_an->finished = true;
			}
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_expr_list_expr(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

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

				dllist_insert_first(sem_an->value->list, expr);

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
				HashTable* symtab = get_current_sym_tab(parser);
				htab_item* func_item = htab_find(symtab, value.token->data.str);
				if (func_item == NULL)
					return EXIT_SEMANTIC_PROG_ERROR;

				// Check parameters
				if (sem_an->value == NULL) {
					// No parameters
					if (func_get_param(func_item, 1) != END_OF_TERMINALS)  // If function has at least 1 param
						return EXIT_SEMANTIC_COMP_ERROR;
				} else if (sem_an->value->value_type == VTYPE_EXPR) {
					// 1 Parameter
					if (!are_types_compatible((token_e) sem_an->value->expr_type, func_get_param(func_item, 1)))
						return EXIT_SEMANTIC_COMP_ERROR;

					// If function has more params
					if (func_get_param(func_item, 2) != END_OF_TERMINALS)
						return EXIT_SEMANTIC_COMP_ERROR;
				} else {
					// 2 and more params
					dllist_activate_last(sem_an->value->list);
					unsigned i;
					SemValue* expr;
					for (i = 1; dllist_active(sem_an->value->list); i++) {
						expr = (SemValue*) dllist_get_active(sem_an->value->list);
						if (!are_types_compatible((token_e) expr->expr_type, func_get_param(func_item, i)))
							return EXIT_SEMANTIC_COMP_ERROR;
					}

					// If function has more params
					if (func_get_param(func_item, i) != END_OF_TERMINALS)
						return EXIT_SEMANTIC_COMP_ERROR;
				}

				IL_ADD(OP_CALL, addr_symbol("", func_item->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

				sem_value_free(sem_an->value);
				SEM_SET_EXPR_TYPE(func_item->func_data->rt);

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

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER)
			{
				symtab = get_current_sym_tab(parser);

				// Check variable redefinition
				if (htab_find(symtab, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put variable in value table
				htab_item* item = htab_lookup(symtab, value.token->data.str);
				if (item == NULL) {
					return EXIT_INTERN_ERROR;
				}

				sem_an->value = sem_value_init();
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				sem_an->value->value_type = VTYPE_ID;
				sem_an->value->id = item;

				IL_ADD(OP_DEFVAR, addr_symbol(get_current_scope_prefix(parser), item->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);

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
						sem_an->value->id->id_data->type = value.token->id;

						SEM_NEXT_STATE(SEM_STATE_ASSIGN);
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_ASSIGN) {
			if (value.value_type == VTYPE_ID) {  // Variable initialization
				const char* prefix = get_current_scope_prefix(parser);
				IL_ADD(OP_MOVE, addr_symbol(prefix, sem_an->value->id->key), addr_symbol(prefix, value.id->key), NO_ADDR, EXIT_INTERN_ERROR);
				sem_an->finished = true;
			} else if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {  // Default initialization
					case TOKEN_EOL:
						switch (sem_an->value->id->id_data->type) {
							case TOKEN_KW_INTEGER:
								IL_ADD(OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->id->key), addr_constant(MAKE_TOKEN_INT(0)), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							case TOKEN_KW_BOOLEAN:
								IL_ADD(OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->id->key), addr_constant(MAKE_TOKEN_BOOL(false)), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							case TOKEN_KW_DOUBLE:
								IL_ADD(OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->id->key), addr_constant(MAKE_TOKEN_REAL(0)), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							case TOKEN_KW_STRING:
								IL_ADD(OP_MOVE, addr_symbol(get_current_scope_prefix(parser), sem_an->value->id->key), addr_constant(MAKE_TOKEN_STRING("")), NO_ADDR, EXIT_INTERN_ERROR);
								break;
							default:
								assert(!"I shouldn't be here");
						}
						sem_an->finished = true;
						break;
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
				if (create_scope(parser) == NULL) {
					return EXIT_INTERN_ERROR;
				}
				SEM_NEXT_STATE(SEM_STATE_SCOPE_END);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_SCOPE_END) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_SCOPE)
			{
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
				IL_ADD(OP_WRITE, addr_symbol(F_LOCAL, value.id->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
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
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

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
						// Create new local symtable
						if (create_scope(parser) == NULL)
							return EXIT_INTERN_ERROR;
						// Function was declared
						SEM_NEXT_STATE(SEM_STATE_DECLARED_VAR_TYPE);
						break;
					}
				}

				// Function was NOT declared
				// add it to symtable
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

		// Check parameters with function declaration
		SEM_STATE(SEM_STATE_DECLARED_VAR_TYPE) {
			static unsigned idx = 1;
			if (value.value_type == VTYPE_ID) {
				switch (value.id->id_data->type) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN: {
						symtab_func = parser->sym_tab_functions;
						htab_item *item = htab_find(symtab_func, sem_an->value->token->data.str);
						// Type in declaration and definition does not match
						if (value.id->id_data->type != func_get_param(item, idx))
							return EXIT_SEMANTIC_PROG_ERROR;
						idx++;
						if (!func_store_param_name(item, value.id->key))
							return EXIT_INTERN_ERROR;
					}
					default:
						break;
				};
			}
				// End of parametr declarations
			else if (value.value_type == VTYPE_TOKEN
					 && value.token->id == TOKEN_RPAR) {
				symtab_func = parser->sym_tab_functions;
				htab_item *item = htab_find(symtab_func, sem_an->value->token->data.str);
				// Function definition did not provide enough parameters
				if (item->func_data->par_num != idx -1)
					return EXIT_SEMANTIC_PROG_ERROR;

				idx = 1;
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
						symtab_func = parser->sym_tab_functions;
						htab_item *item = htab_find(symtab_func, sem_an->value->token->data.str);
						if (!func_store_param_name(item, value.id->key))
							return EXIT_INTERN_ERROR;
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

		SEM_STATE(SEM_STATE_DECLARED_RETURN_TYPE) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN:
					{
						symtab_func = parser->sym_tab_functions;
						htab_item* item = htab_find(symtab_func, sem_an->value->token->data.str);
						if (item->func_data->rt != value.token->id)
							return EXIT_SEMANTIC_PROG_ERROR;
						func_set_def(item);

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
						symtab_func = parser->sym_tab_functions;
						htab_item* item = htab_find(symtab_func, sem_an->value->token->data.str);
						func_set_rt(item, value.token->id);
						func_set_def(item);

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
					SEM_NEXT_STATE(SEM_STATE_EOL);
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

int sem_do_loop(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	SEM_ACTION_CHECK;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_EOL)
			{
				if (create_scope(parser) == NULL) {
					return EXIT_INTERN_ERROR;
				}
				SEM_NEXT_STATE(SEM_STATE_DO_LOOP);
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_DO_LOOP) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_LOOP)
			{
				delete_scope(parser);
				SEM_NEXT_STATE(SEM_STATE_EOL);
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
