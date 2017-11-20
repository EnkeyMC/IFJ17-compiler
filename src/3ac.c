#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "3ac.h"

static const char* opcodes_str[] = {
	FOREACH_OPCODE(GENERATE_STRING)
};

DLList* instruction_list = NULL;

const char* scope_prefix[3] = {"GF@", "LF@", "TF@"};

Instruction* instruction_init(opcode_e operation, Address addr1, Address addr2, Address addr3) {
	Instruction* inst = (Instruction*) malloc(sizeof(Instruction));
	if (inst == NULL)
		return NULL;

	if (addr1.type == ADDR_TYPE_ERROR ||
		addr2.type == ADDR_TYPE_ERROR ||
		addr3.type == ADDR_TYPE_ERROR)
	{
		address_free(addr1);
		address_free(addr2);
		address_free(addr3);
		free(inst);
		return NULL;
	}

	inst->operation = operation;
	inst->addresses[0] = addr1;
	inst->addresses[1] = addr2;
	inst->addresses[2] = addr3;

	return inst;
}

void instruction_free(void* inst) {
	Instruction* instruction = (Instruction*) inst;
	for (int i = 0; i < MAX_ADDRESSES; i++) {
		address_free(instruction->addresses[i]);
	}

	free(inst);
}

Address addr_symbol(const char* prefix, const char* symbol) {
	Address addr;

	addr.symbol = (char*) malloc(sizeof(char) * (strlen(prefix) + strlen(symbol) + 1));
	if (addr.symbol == NULL) {
		addr.type = ADDR_TYPE_ERROR;
		return addr;
	}

	strcpy(addr.symbol, prefix);
	strcat(addr.symbol, symbol);
	addr.type = ADDR_TYPE_SYMBOL;

	return addr;
}

Address addr_constant(Token token) {
	Address addr;

	addr.constant = token_copy(&token);
	if (addr.constant == NULL) {
		addr.type = ADDR_TYPE_ERROR;
		return addr;
	}

	addr.type = ADDR_TYPE_CONST;

	return addr;
}

void address_free(Address addr) {
	switch (addr.type) {
		case ADDR_TYPE_SYMBOL:
			free(addr.symbol);
			break;
		case ADDR_TYPE_CONST:
			token_free(addr.constant);
			break;
		default:
			break;
	}
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

bool il_add(Instruction* instruction) {
	if (instruction == NULL)
		return false;

	// Just for the sake of tests
	if (instruction_list == NULL)
		return true;

	return dllist_insert_last(instruction_list, instruction);
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
						printf("string@%s ", instruction->addresses[i].constant->data.str);
						break;
					case TOKEN_INT:
						printf("int@%d ", instruction->addresses[i].constant->data.i);
						break;
					case TOKEN_REAL:
						printf("float@%g ", instruction->addresses[i].constant->data.d);
						break;
					case TOKEN_KW_TRUE:
						printf("bool@true");
						break;
					case TOKEN_KW_FALSE:
						printf("bool@false");
						break;
					default:
						assert(!"I shouldn't be here");
						break;
				}
				break;
			default:
				break;
		}
	}

	printf("\n");
}

void generate_code() {
	puts(".IFJcode17");
	if (!dllist_empty(instruction_list)) {
		dllist_activate_first(instruction_list);

		Instruction* instruction;
		while (dllist_active(instruction_list)) {
			instruction = (Instruction*) dllist_get_active(instruction_list);
			assert(instruction != NULL);
			print_instruction(instruction);
			dllist_succ(instruction_list);
		}
	}
}
