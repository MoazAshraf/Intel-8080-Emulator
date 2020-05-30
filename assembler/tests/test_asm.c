// test_asm.c: tests for assemble() in assemble.c
//
// Copyright (c) 2020 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdio.h>
#include <stdlib.h>
#include "../src/asm.h"

void test_assemble(char *);
int str_to_int(char *s, int b);

char *prog;

int main()
{
    prog = "test";

    test_assemble("IF 1 INX 0 ENDIF");
    test_assemble("B SET 0  INX B");
    test_assemble("B SET 0  B SET 2  INX B");
    test_assemble("BLA EQU 0  INX BLA");

    // pseudo-instructions
    test_assemble("DS 5");
    test_assemble("DB 12, 6, 'A', '9'-'0', 'B'");
    test_assemble("DW 01FAH, 6, 'AB', '9'+256, 'AA'");
    test_assemble("NOP NOP INR 0 END NOP NOP INR 0");
    test_assemble("NOP NOP INR 0 ORG 0FFH NOP NOP inr 0 ORG 0 SHLD 0A6FFH");

    // instructions
    test_assemble("NOP");
    test_assemble("SHLD 0a6ffh");
    test_assemble("LXI 0, 2*3");
    test_assemble("LXI 2, 2*3");
    test_assemble("INR 2*3-5+2/4");
    test_assemble("INR 1 OR 0");
    test_assemble("INR 1 AND 0");
    test_assemble("INR 1 AND 1");
    test_assemble("INR 1 XOR 1");
    test_assemble("INR ((5+3)*2) AND (5+3*2)");
    test_assemble("INR not 0FFFFH");

    // errors
    // test_assemble("DB");
    // test_assemble("LXI -5, 2*3");
    // test_assemble("INR 5+3*2");
    // test_assemble("INR (5+3)*2");
    // test_assemble("INR 5+(3*2)");
    // test_assemble("INR ((5+3)*2)");
    // test_assemble("INR ((5+3)*2) OR (5+3*2)");
    // test_assemble("INR not 0");
    // test_assemble("B EQU 0  B EQU 2  INX B");
    // test_assemble("B EQU 0  B SET 2  INX B");

    return 0;
}

int get_statements(char *, Statement *);
int assemble(Statement *, int, char *);
void print_hexdump(uint8_t *, int);

void test_assemble(char *s)
{
    // get statements
    Statement *statements = (Statement *) calloc(MAX_STMNTS, sizeof(Statement));
    int nstmnt = get_statements(s, statements);

    // assemble
    char outbuf[MAX_PROG];
    int outsize = assemble(statements, nstmnt, outbuf);

    // print hex dump of output
    print_hexdump(outbuf, outsize);
}

void print_hexdump(uint8_t buf[], int size)
{
    int pc;
    for (int i = 0, pc = 0; i < size; i += 16) {
        printf("%08x ", i);
        for (pc = i; pc < i + 16 && pc < size; pc++) {
            printf("%02x ", buf[pc]);
        }
        putchar('\n');
    }
    putchar('\n');
}