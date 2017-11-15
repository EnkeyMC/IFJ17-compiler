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
	sem_an->symbol = NULL;

	return sem_an;
}

void sem_an_free(void* sem_an) {
	if (((SemAnalyzer*) sem_an)->symbol != NULL) {
		token_free(((SemAnalyzer*) sem_an)->symbol);
	}

	free(sem_an);
}

static HashTable* get_current_sym_tab(Parser* parser) {
	// Get local symbol table
	HashTable* symtab = (HashTable*) stack_top(parser->sym_tab_stack);
	if (symtab == NULL) {
		symtab = parser->sym_tab_global;  // If there is no local symbol table, use global
	}

	return symtab;
}

// SEMANTIC FUNCTIONS

int sem_var_decl(SemAnalyzer* sem_an, Parser* parser, Token* token) {
	assert(sem_an != NULL);
	assert(parser != NULL);
	assert(token != NULL);

	HashTable* symtab = NULL;

	switch (sem_an->state) {
		case SEM_STATE_START:
			if (token->id == TOKEN_IDENTIFIER) {
				sem_an->symbol = token_copy(token);
				if (sem_an->symbol == NULL)
					return EXIT_INTERN_ERROR;

				symtab = get_current_sym_tab(parser);

				// Check variable redefinition
				if (htab_find(symtab, token->str) != NULL) {
					return EXIT_SEMANTIC_PROG_ERROR;
				}

				// Put variable in symbol table
				if (htab_lookup(symtab, token->str) == NULL) {
					return EXIT_INTERN_ERROR;
				}

				sem_an->state = SEM_STATE_VAR_TYPE;  // Set next state
			}
			break;

		case SEM_STATE_VAR_TYPE:
			switch (token->id) {
				case TOKEN_KW_INTEGER:
				case TOKEN_KW_DOUBLE:
				case TOKEN_KW_STRING:
				case TOKEN_KW_BOOLEAN:
					{
						symtab = get_current_sym_tab(parser);
						htab_item_t* item = htab_lookup(symtab, sem_an->symbol->str);
						item->type = token->id;

						sem_an->state = SEM_STATE_EOL;
					}
				default:
					break;
			}
			break;

		case SEM_STATE_EOL:
			switch (token->id) {
				case TOKEN_EOL:
					sem_an->finished = true;
					break;
				default:
					break;
			}
			break;
		default:
			return EXIT_INTERN_ERROR;
	}

	return EXIT_SUCCESS;
}
