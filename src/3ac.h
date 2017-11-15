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

#define MAX_ADDRESSES 3

/**
 * Enum of operation codes
 */
typedef enum {
    FOREACH_OPCODE(GENERATE_ENUM)
} opcode_e;

/**
 * Enum of operand types, used to determine the value of operand
 */
typedef enum {
    ADDR_TYPE_SYMBOL,  // Symbol from symtable
    ADDR_TYPE_CONST,  // Constant
    ADDR_TYPE_EMPTY  // No operand
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

extern DLList* instruction_list;  /// Global instruction list

/**
 * Free instruction
 * @param inst Instruction
 */
void instruction_free(void* inst);

/**
 * Initialize instruction list
 * @return true on success, false otherwise
 */
bool il_init();

/**
 * Free instruction list
 */
void il_free();

/**
 * Generate 3 address code
 */
void generate_code();

#endif //IFJ17_COMPILER_3AC_H
