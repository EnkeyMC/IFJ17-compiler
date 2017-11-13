#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "sem_analyzer.h"
#include "error_code.h"
#include "token.h"
#include "stack.h"
#include "symtable.h"
#include "parser.h"


SemAnalyzer* sem_an_init(semantic_action_f sem_action) {
	SemAnalyzer* sem_an = (SemAnalyzer*) malloc(sizeof(SemAnalyzer));
	if (sem_an == NULL)
		return NULL;

	sem_an->sem_action = sem_action;
	sem_an->state = SEM_STATE_START;

	return sem_an;
}

void sem_an_free(SemAnalyzer* sem_an) {
	free(sem_an);
}

// SEMANTIC FUNCTIONS

int sem_var_decl(SemAnalyzer* sem_an, struct parser_t* parser, Token* token) {
	assert(sem_an != NULL);
	assert(parser != NULL);
	assert(token != NULL);

	switch (sem_an->state) {
		case SEM_STATE_START:
			assert(token->id == TOKEN_IDENTIFIER);

			// Get local symbol table
			HashTable* symtab = (HashTable*) stack_top(parser->sym_tab_stack);
			if (symtab == NULL) {
				symtab = parser->sym_tab_global;  // If there is no local symbol table, use global
			}

			// Check variable redefinition
			if (htab_find(symtab, token->str) != NULL) {
				return EXIT_SEMANTIC_PROG_ERROR;
			}

			// Put variable in symbol table
			if (htab_lookup(symtab, token->str) == NULL) {
				return EXIT_INTERN_ERROR;
			}

			break;
		default:
			return EXIT_INTERN_ERROR;
	}

	return EXIT_SUCCESS;
}
