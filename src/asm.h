// asm.h: Intel 8080 Assembler header file
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#ifndef ASM_H
#define ASM_H

#include <stddef.h>
#include <stdint.h>

// argument operand type
typedef enum OpType {
    OP_HEX, OP_DEC, OP_OCT, OP_BIN, OP_PC, OP_ASCII, OP_LABEL, OP_EXPR
} OpType;

// argument expression definition
typedef struct Expr {
    OpType type;        // expression type
    char *value;        // expression value
    char *oper;         // operator
    struct Expr *op1;   // first operand
    struct Expr *op2;   // second operand
} Expr;

// program statement definition
typedef struct Statement {
    uint16_t pc;     // program counter value
    char *label;    // label field; NULL if not specified
    char *instr;    // instruction field; NULL if not specified
    Expr *arg1;     // first argument field; NULL if not specified
    Expr *arg2;     // second argument field; NULL if not specified
} Statement;

// instruction definition
typedef struct Instr {
    uint8_t opcode; // operation code
    int size;       // equivalent machine code size in bytes
    char *mnem;     // uppercase mnemonic
    int nargs;      // number of arguments
    char *arg1;     // first argument value
    char *arg2;     // second argument value; NULL if doesn't affect opcode
} Instr;

#define MAX_STMNTS  0x20000 // maximum number of program statements
#define WORD_LEN    256     // maximum length of a word
#define LABEL_LEN   5       // maximum label length

#endif  // ASM_H