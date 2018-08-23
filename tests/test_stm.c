// test_stm.c: tests for get_statements() in tokenize.c
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdlib.h>
#include <stdio.h>
#include "../src/asm.h"

char *prog;

void test_statements(char *st);

int main()
{
    prog = "test";
    test_statements("LXI A, B");
    // test_statements("NOP");
    // test_statements("LABEL: NOP LB:");
    // test_statements("LABEL: LABEL:");
    // test_statements("LABEL: LABEL");
    // test_statements("LABEL: NOP");
    // test_statements("LB: LABEL: NOP LXI LB2: ; comment");
    // test_statements("; comment");
    // test_statements("LABEL: BLA");
    // test_statements("NOP:");
    // test_statements("@LABEL:\n@SBPDF:");
    // test_statements("1LABEL:");
}

int get_statements(char [], Statement []);

void test_statements(char *st)
{
    Statement *statements = (Statement *) calloc(MAX_STMNTS, sizeof(Statement));

    int nstmnt = get_statements(st, statements);

    for (int i = 0; i < nstmnt; i++) {
        printf("%04x\t", statements[i].pc);
        if (statements[i].label)
            printf("%s:", statements[i].label);
        if (statements[i].instr)
            printf("\t%s", statements[i].instr);
        putchar('\n');
    }
}