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
    TOK_UNAOPER, TOK_BINOPER, TOK_PAREN, TOK_COMMA
} TokType;

// argument token definition
typedef struct Token {
    TokType type;   // token type
    char *str;      // token string
} Token;

// instruction argument definition
typedef struct Arg {
    Token *toks;    // token list
    int ntoks;      // argument token count
} Arg;

// instruction argument list definition
typedef struct Args {
    int nargs;      // argument count
    Arg *args;      // argument list
} Args;

// program statement definition
typedef struct Statement {
    uint16_t pc;    // program counter value
    char *label;    // label
    char *name;     // for pseudo-instructions
    char *instr;    // instruction or pseudo-instruction
    Args args;      // arguments
} Statement;

// instruction definition
typedef struct Instr {
    uint8_t opcode; // operation code
    int size;       // equivalent machine code size in bytes
    int nargs;      // number of arguments
    char *mnem;     // mnemonic
    char *arg1;     // first argument value
    char *arg2;     // second argument value; NULL if doesn't affect opcode
} Instr;

// pseudo-instruction definition
typedef struct Pseudo {
    int reqname;    // requires a name?
    char *mnem;     // mnemonic
    int nargs;      // number of arguments ( 3 = req. list, >3 = optional list)
} Pseudo;

// operator definition
typedef struct Oper {
    char *str;      // operator string
    int prec;       // operator's precedence
} Oper;

#define MAX_STMNTS  0x20000 // maximum number of program statements
#define MAX_WORD    256     // maximum length of a word
#define MAX_LABEL   5       // maximum label length
#define MAX_TOKENS  256     // maximum number of argument expression tokens
#define MAX_ARGS    256     // maximum number of arguments in a list

// is c a word character?
#define iswdchr(c)      (isalnum(c) || (c) == '@' || (c) == '?')

// is t an operand token type?
#define isoperand(t)    ((t) == TOK_HEX         \
                         || (t) == TOK_DEC      \
                         || (t) == TOK_OCT      \
                         || (t) == TOK_BIN      \
                         || (t) == TOK_PC       \
                         || (t) == TOK_ASCII    \
                         || (t) == TOK_LABEL)

#endif  // ASM_H