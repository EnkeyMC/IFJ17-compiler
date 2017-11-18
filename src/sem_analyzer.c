#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "sem_analyzer.h"
#include "error_code.h"
#include "token.h"
#include "stack.h"
#include "parser.h"


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

// SEMANTIC FUNCTIONS

int sem_var_decl(SemAnalyzer* sem_an, Parser* parser, SemValue value) {
	assert(sem_an != NULL);
	assert(parser != NULL);

	HashTable* symtab = NULL;

	switch (sem_an->state) {
		case SEM_STATE_START:
			if (value.value_type == VTYPE_TOKEN
				&& value.token->id == TOKEN_IDENTIFIER)
			{
				sem_an->value = sem_value_copy(&value);
				if (sem_an->value == NULL)
					return EXIT_INTERN_ERROR;

				symtab = get_current_sym_tab(parser);

				// Check variable redefinition
				if (htab_find(symtab, value.token->str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put variable in value table
				if (htab_lookup(symtab, value.token->str) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				sem_an->state = SEM_STATE_VAR_TYPE;  // Set next state
			}
			break;

		case SEM_STATE_VAR_TYPE:
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_KW_INTEGER:
					case TOKEN_KW_DOUBLE:
					case TOKEN_KW_STRING:
					case TOKEN_KW_BOOLEAN:
					{
						symtab = get_current_sym_tab(parser);
						htab_item* item = htab_lookup(symtab, sem_an->value->token->str);
						item->id_data->type = value.token->id;

						sem_an->state = SEM_STATE_EOL;
					}
					default:
						break;
				}
			}
			break;

		case SEM_STATE_EOL:
			if (value.value_type == VTYPE_TOKEN) {
				switch (value.token->id) {
					case TOKEN_EOL:
						sem_an->finished = true;
						break;
					default:
						break;
				}
			}
			break;
		default:
			return EXIT_INTERN_ERROR;
	}

	return EXIT_SUCCESS;
}
