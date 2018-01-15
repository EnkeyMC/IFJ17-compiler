/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "3ac.h"
#include "debug.h"
#include "memory_manager.h"

static const char* opcodes_str[] = {
	FOREACH_OPCODE(GENERATE_STRING) ""
};

DLList* main_il = NULL;
DLList* global_il = NULL;
DLList* func_il = NULL;

const char* scope_prefix[3] = {"GF@", "LF@", "TF@"};

Instruction* instruction_init(opcode_e operation, Address addr1, Address addr2, Address addr3) {
	Instruction* inst = (Instruction*) mm_malloc(sizeof(Instruction));

	if (addr1.type == ADDR_TYPE_ERROR ||
		addr2.type == ADDR_TYPE_ERROR ||
		addr3.type == ADDR_TYPE_ERROR)
	{
		address_free(addr1);
		address_free(addr2);
		address_free(addr3);
		mm_free(inst);
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

	mm_free(inst);
}

Address addr_symbol(const char* prefix, const char* symbol) {
	Address addr;

	addr.symbol = (char*) mm_malloc(sizeof(char) * (strlen(prefix) + strlen(symbol) + 1));

	strcpy(addr.symbol, prefix);
	strcat(addr.symbol, symbol);
	addr.type = ADDR_TYPE_SYMBOL;

	return addr;
}

Address addr_constant(Token token) {
	Address addr;

	addr.constant = token_copy(&token);

	addr.type = ADDR_TYPE_CONST;

	return addr;
}

void address_free(Address addr) {
	switch (addr.type) {
		case ADDR_TYPE_SYMBOL:
			mm_free(addr.symbol);
			break;
		case ADDR_TYPE_CONST:
			token_free(addr.constant);
			break;
		default:
			break;
	}
}

void il_init() {
	main_il = dllist_init(instruction_free);
	func_il = dllist_init(instruction_free);
	global_il = dllist_init(instruction_free);
}

void il_free() {
	dllist_free(main_il);
	dllist_free(func_il);
	dllist_free(global_il);
}

void il_add(DLList* il, Instruction* instruction) {
	if (instruction == NULL)
		return;

	// Just for the sake of tests
	if (il == NULL)
		return;

	dllist_insert_last(il, instruction);
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
}

void generate_code() {
	puts(".IFJcode17");
	puts("# SECTION GLOBAL");
	dllist_activate_first(global_il);

	Instruction* instruction;
	while (dllist_active(global_il)) {
		instruction = (Instruction*) dllist_get_active(global_il);
		assert(instruction != NULL);
		print_instruction(instruction);
		printf("\n");
		dllist_succ(global_il);
	}
	printf("\n\n");

	puts("# SECTION MAIN");
	dllist_activate_first(main_il);
	
	while (dllist_active(main_il)) {
		instruction = (Instruction*) dllist_get_active(main_il);
		assert(instruction != NULL);
		print_instruction(instruction);
		printf("\n");
		dllist_succ(main_il);
	}
	// Jump to end to skip functions
	puts("JUMP PROGRAM_END");
	printf("\n\n");

	puts("# SECTION FUNCTIONS");
	dllist_activate_first(func_il);

	while (dllist_active(func_il)) {
		instruction = (Instruction*) dllist_get_active(func_il);
		assert(instruction != NULL);
		print_instruction(instruction);
		printf("\n");
		dllist_succ(func_il);
	}
	puts("LABEL PROGRAM_END");
}

void instruction_debug(void *inst) {
	Instruction* instruction = (Instruction*) inst;
	debug("Instruction@%p: {", instruction);
	if (instruction != NULL) {
		debug("%s ", opcodes_str[instruction->operation]);

		for (int i = 0; i < MAX_ADDRESSES; ++i) {
			switch (instruction->addresses[i].type) {
				case ADDR_TYPE_SYMBOL:
					debug("%s ", instruction->addresses[i].symbol);
					break;
				case ADDR_TYPE_CONST:
					switch (instruction->addresses[i].constant->id) {
						case TOKEN_STRING:
							debug("string@%s ", instruction->addresses[i].constant->data.str);
							break;
						case TOKEN_INT:
							debug("int@%d ", instruction->addresses[i].constant->data.i);
							break;
						case TOKEN_REAL:
							debug("float@%g ", instruction->addresses[i].constant->data.d);
							break;
						case TOKEN_KW_TRUE:
							debugs("bool@true");
							break;
						case TOKEN_KW_FALSE:
							debugs("bool@false");
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
	}
	debugs("}");
}
