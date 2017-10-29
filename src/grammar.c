#include <stdarg.h>
#include <malloc.h>

#include "grammar.h"
#include "token.h"

#define ADD_RULE(nt, ...) if (!grammar_add_rule(nt, __VA_ARGS__)) { grammar_free(); return false; }

static bool grammar_add_rule(non_terminal_e nt, int va_num, ...) {
	Rule* rule = (Rule*) malloc(sizeof(Rule));
	if (rule == NULL) {
		return false;
	}

	rule->production = (int*) malloc(sizeof(int) * va_num + 1);
	if (rule->production == NULL) {
		free(rule);
		return false;
	}

	va_list va_args;
	va_start(va_args, va_num);
	int i;
	for (i = 0; i < va_num; i++) {
		rule->production[i] = va_arg(va_args, int);
	}

	rule->production[i] = END_OF_RULE;

	va_end(va_args);

	if (grammar[nt] != NULL) {
		struct rule_t* last_rule = grammar[nt];

		while (last_rule->next != NULL) {
			last_rule = last_rule->next;
		}

		last_rule->next = rule;
	} else {
		grammar[nt] = rule;
	}

	return true;
}

static void rule_free(Rule* rule) {
	if (rule->production != NULL)
		free(rule->production);
	free(rule);
}

bool grammar_init() {
	ADD_RULE(NT_PROGRAM, NT_LINES_N, TOKEN_EOF);
	// TODO rest of the rules

	return true;
}

void grammar_free() {
	struct rule_t* tmp_rule, *to_free_rule;
	for (int i = 0; i < NT_ENUM_SIZE; i++) {
		tmp_rule = grammar[i];
		if (tmp_rule != NULL) {
			to_free_rule = tmp_rule;
			tmp_rule = tmp_rule->next;
			rule_free(to_free_rule);
		}
	}
}
