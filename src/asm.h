// asm.h: Intel 8080 Assembler header file
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#ifndef ASM_H
#define ASM_H

#include <stddef.h>
#include <stdint.h>

// argument token type
typedef enum TokType {
    TOK_HEX, TOK_DEC, TOK_OCT, TOK_BIN, TOK_PC, TOK_ASCII, TOK_LABEL, TOK_INSTR,
    TOK_UNAOPER, TOK_BINOPER, TOK_PAREN
} TokType;

// argument token definition
typedef struct Token {
    TokType type;   // token type
    char *str;      // token string
} Token;

// program statement definition
typedef struct Statement {
    uint16_t pc;    // program counter value
    char *label;    // label
    char *instr;    // instruction or pseudo-instruction
    Token *arg1;    // first argument expression
    Token *arg2;    // second argument expression
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

// operator definition
typedef struct Oper {
    char *str;      // operator string
    int prec;       // operator's precedence
} Oper;

#define MAX_STMNTS  0x20000 // maximum number of program statements
#define MAX_WORD    256     // maximum length of a word
#define MAX_LABEL   5       // maximum label length

#endif  // ASM_H