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
	}

	free(value);
}

static HashTable* get_current_sym_tab(Parser* parser) {
	// Get local value table
	HashTable* symtab = (HashTable*) dllist_get_first(parser->sym_tab_stack);
	if (symtab == NULL) {
		symtab = parser->sym_tab_global;  // If there is no local value table, use global
	}

	return symtab;
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
// SEMANTIC FUNCTIONS

/**
 * Called at the end of evaluating expression to prepare value for parent SemAnalyzer
 * @param sem_an SemAnalyzer
 * @param parser Parser
 * @param value SemValue
 * @return exit code
 */
int sem_expr_end(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	assert(sem_an != NULL);
	assert(parser != NULL);

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			assert(value.value_type == VTYPE_ID);
			sem_an->value = sem_value_copy(&value);

			IL_ADD(OP_DEFVAR, addr_symbol(F_LOCAL, sem_an->value->id->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
			IL_ADD(OP_POPS, addr_symbol(F_LOCAL, sem_an->value->id->key), NO_ADDR, NO_ADDR, EXIT_INTERN_ERROR);
			sem_an->finished = true;
		} END_STATE;

		SEM_ERROR_STATE;
	}

	return EXIT_SUCCESS;
}

int sem_var_decl(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	assert(sem_an != NULL);
	assert(parser != NULL);

	HashTable* symtab = NULL;

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER) {
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				symtab = get_current_sym_tab(parser);

				// Check variable redefinition
				if (htab_find(symtab, value.token->data.str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put variable in value table
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

						SEM_NEXT_STATE(SEM_STATE_EOL);
					}
					default:
						break;
				}
			}
		} END_STATE;

		SEM_STATE(SEM_STATE_EOL) {
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_EOL:
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
	assert(sem_an != NULL);
	assert(parser != NULL);

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
	assert(sem_an != NULL);
	assert(parser != NULL);

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
	assert(sem_an != NULL);
	assert(parser != NULL);

	SEM_FSM {
		SEM_STATE(SEM_STATE_START) {
			if (value.value_type == VTYPE_TOKEN &&
				value.token->id == TOKEN_KW_SCOPE)
			{
				create_scope(parser);
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
	assert(sem_an != NULL);
	assert(parser != NULL);

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
