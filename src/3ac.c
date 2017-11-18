#include <malloc.h>
#include <assert.h>
#include "3ac.h"

static const char* opcodes_str[] = {
	FOREACH_OPCODE(GENERATE_STRING)
};

DLList* instruction_list;

void instruction_free(void* inst) {
	Instruction* instruction = inst;
	for (int i = 0; i < MAX_ADDRESSES; i++) {
		switch (instruction->addresses[i].type) {
			case ADDR_TYPE_SYMBOL:
				free(instruction->addresses[i].symbol);
				break;
			case ADDR_TYPE_CONST:
				token_free(instruction->addresses[i].constant);
				break;
			default:
				break;
		}
	}

	free(inst);
}

bool il_init() {
	instruction_list = dllist_init(instruction_free);
	if (instruction_list == NULL)
		return false;
	return true;
}

void il_free() {
	dllist_free(instruction_list);
}

static void print_instruction(Instruction* instruction) {
	assert(instruction != NULL);

	printf("%s ", opcodes_str[instruction->operation]);

	for (int i = 0; i < MAX_ADDRESSES; ++i) {
		switch (instruction->addresses[i].type) {
			case ADDR_TYPE_SYMBOL:
				printf("%s ", instruction->addresses[i].symbol);
				break;
			case ADDR_TYPE_CONST:
				switch (instruction->addresses[i].constant->id) {
					case TOKEN_STRING:
						printf("string@%s ", instruction->addresses[i].constant->str);
						break;
					case TOKEN_INT:
						printf("int@%d ", instruction->addresses[i].constant->i);
						break;
					case TOKEN_REAL:
						printf("float@%g ", instruction->addresses[i].constant->d);
						break;
					default:
						assert("I shouldn't be here");
						break;
				}
				break;
			default:
				break;
		}
	}
}

void generate_code() {
	puts(".IFJcode17");
	if (!dllist_empty(instruction_list)) {
		dllist_activate_first(instruction_list);

		Instruction* instruction;
		while (dllist_active(instruction_list)) {
			instruction = dllist_get_active(instruction_list);
			assert(instruction != NULL);
			print_instruction(instruction);
			dllist_succ(instruction_list);
		}
	}
}
