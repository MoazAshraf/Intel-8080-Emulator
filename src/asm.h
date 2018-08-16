// asm.h: Intel 8080 Assembler header file
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#ifndef ASM_H
#define ASM_H

#include <stdint.h>

typedef struct Expr {
    uint16_t value;     // expression value (op1 and op2 are ignored)
    char *oper;         // operator
    struct Expr *op1;   // first operand; NULL if not specified
    struct Expr *op2;   // second operand; NULL if not specified
} Expr;

typedef struct Statement {
    uint8_t pc;     // program counter value
    char *label;    // label field; NULL if not specified
    char *instr;    // instruction field; NULL if not specified
    Expr *arg1;     // first argument field; NULL if not specified
    Expr *arg2;     // second argument field; NULL if not specified
} Statement;

#define MAXSTMNTS 0x20000 // maximum number of program statements

#endif  // ASM_H