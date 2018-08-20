// mnemonics.c: Intel 8080 assembly mnemonic list definitions
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdlib.h>
#include "asm.h"

// instructions are mnemonics with machine code equivalents (opcodes)
Instr instrs[] = {
    {0x00, 1, "NOP", 0},
    {0x01, 3, "LXI", 2, "B", NULL},
    {0x02, 1, "STAX", 1, "B"},
    {0x03, 1, "INX", 1, "B"},
    {0x04, 1, "INR", 1, "B"},
};

// unary operators
Oper unaopers[] = {
    {"+", 1}, {"-", 1},
    {"NOT", 2}
};

const int N_UNAOPERS = sizeof unaopers / sizeof(Oper);

// binary operators
Oper binopers[] = {
    {"*", 0}, {"/", 0}, {"MOD", 0}, {"SHR", 0}, {"SHL", 0},
    {"+", 1}, {"-", 1},
    {"AND", 3},
    {"OR", 4}, {"XOR", 4}
};

const int N_BINOPERS = sizeof binopers / sizeof(Oper);