/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_3AC_H
#define IFJ17_COMPILER_3AC_H

// Operation code with string representation, all codes are prefixed by OP_
#include "token.h"
#include "dllist.h"

#define FOREACH_OPCODE(OPCODE) \
    OPCODE(MOVE) \
    OPCODE(CREATEFRAME) \
    OPCODE(PUSHFRAME) \
    OPCODE(POPFRAME) \
    OPCODE(DEFVAR) \
    OPCODE(CALL) \
    OPCODE(RETURN) \
    OPCODE(PUSHS) \
    OPCODE(POPS) \
    OPCODE(CLEARS) \
    OPCODE(ADD) \
    OPCODE(SUB) \
    OPCODE(MUL) \
    OPCODE(DIV) \
    OPCODE(ADDS) \
    OPCODE(SUBS) \
    OPCODE(MULS) \
    OPCODE(DIVS) \
    OPCODE(LT) \
    OPCODE(GT) \
    OPCODE(EQ) \
    OPCODE(LTS) \
    OPCODE(GTS) \
    OPCODE(EQS) \
    OPCODE(AND) \
    OPCODE(OR) \
    OPCODE(NOT) \
    OPCODE(ANDS) \
    OPCODE(ORS) \
    OPCODE(NOTS) \
    OPCODE(INT2FLOAT) \
    OPCODE(FLOAT2INT) \
    OPCODE(FLOAT2R2EINT) \
    OPCODE(FLOAT2R2OINT) \
    OPCODE(INT2CHAR) \
    OPCODE(STRI2INT) \
    OPCODE(INT2FLOATS) \
    OPCODE(FLOAT2INTS) \
    OPCODE(FLOAT2R2EINTS) \
    OPCODE(FLOAT2R2OINTS) \
    OPCODE(INT2CHARS) \
    OPCODE(STRI2INTS) \
    OPCODE(READ) \
    OPCODE(WRITE) \
    OPCODE(CONCAT) \
    OPCODE(STRLEN) \
    OPCODE(GETCHAR) \
    OPCODE(SETCHAR) \
    OPCODE(TYPE) \
    OPCODE(LABEL) \
    OPCODE(JUMP) \
    OPCODE(JUMPIFEQ) \
    OPCODE(JUMPIFNEQ) \
    OPCODE(JUMPIFEQS) \
    OPCODE(JUMPIFNEQS) \
    OPCODE(BREAK) \
    OPCODE(DPRINT)

#define GENERATE_ENUM(ENUM) OP_##ENUM,
#define GENERATE_STRING(STR) #STR,

#define NO_ADDR ((Address) {.type = ADDR_TYPE_EMPTY, {NULL}})
#define F_GLOBAL scope_prefix[0]
#define F_LOCAL scope_prefix[1]
#define F_TMP scope_prefix[2]

// Helper macros for adding instructions
#define IL_ADD(il, op, addr1, addr2, addr3) il_add(il, instruction_init(op, addr1, addr2, addr3))
#define IL_ADD_SPACE(il) il_add(il, instruction_init(OP_SPACE, NO_ADDR, NO_ADDR, NO_ADDR))
#define MAKE_TOKEN_INT(num) token_make(TOKEN_INT, (union token_data){.i = (num)})
#define MAKE_TOKEN_REAL(real) token_make(TOKEN_REAL, (union token_data){.d = (real)})
#define MAKE_TOKEN_STRING(string) token_make_str(string)
#define MAKE_TOKEN_BOOL(boolean) token_make((boolean) ? TOKEN_KW_TRUE : TOKEN_KW_FALSE, (union token_data){.i = 0})


#define MAX_ADDRESSES 3

/**
 * Enum of operation codes
 */
typedef enum {
    FOREACH_OPCODE(GENERATE_ENUM) OP_SPACE
} opcode_e;

/**
 * Enum of operand types, used to determine the value of operand
 */
typedef enum {
    ADDR_TYPE_SYMBOL,  // Symbol from symtable
    ADDR_TYPE_CONST,  // Constant
    ADDR_TYPE_EMPTY,  // No operand
    ADDR_TYPE_ERROR  // Error occurred allocating content of address
} addr_type_e;

/**
 * Instruction address data type
 */
typedef struct address_t {
    addr_type_e type;  /// Type of address, used to determine the value of address

    union {
        char* symbol;  /// Symbol identifier with frame prefix (GF@, LF@, TF@)
        Token* constant;  /// Constant in token form
    };
} Address;

/**
 * Instruction data type
 */
typedef struct instruction_t {
    opcode_e operation;  /// Instruction operation code
    Address addresses[MAX_ADDRESSES];  /// Array of addresses
} Instruction;

extern const char* scope_prefix[3];  /// Array of scope prefixes, use macros F_LOCAL,...

extern DLList* main_il;  /// Global instruction list for main
extern DLList* func_il;  /// Global instruction list for functions
extern DLList* global_il;  /// Global instruction list for global variables

/**
 * Initialize new intruction
 * @param addr1 Address 1
 * @param addr2 Address 2
 * @param addr3 Address 3
 * @return new instruction
 */
Instruction* instruction_init(opcode_e operation, Address addr1, Address addr2, Address addr3);

/**
 * Free instruction
 * @param inst Instruction
 */
void instruction_free(void* inst);

/**
 * Create new address for given symbol
 * @param prefix symbol prefix ("GF@", "LF@", "TF@")
 * @param symbol identifier (will be copied)
 * @return Address
 */
Address addr_symbol(const char* prefix, const char* symbol);

/**
 * Create new address for given constant
 * @param token constant in Token form (content will be copied)
 * @return Address
 */
Address addr_constant(Token token);

/**
 * Free the content of address
 * @param addr Address
 */
void address_free(Address addr);

/**
 * Initialize instruction list
 */
void il_init();

/**
 * Free instruction list
 */
void il_free();

/**
 * Adds instruction to instruction list
 * @param instruction Instruction to add
 */
void il_add(DLList* il, Instruction* instruction);

/**
 * Generate 3 address code
 */
void generate_code();

/**
 * Print instruction
 * @param inst Instruction
 */
void instruction_debug(void *inst);

#endif //IFJ17_COMPILER_3AC_H
