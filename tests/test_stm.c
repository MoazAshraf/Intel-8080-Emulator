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

    // instructions
    test_statements("NOP");
    test_statements("INX B");
    test_statements("MOV A, B");
    test_statements("HLT");
    test_statements("DAD SP");
    test_statements("INR A");

    // labels
    test_statements("LABEL: NOP");
    test_statements("@LB1: INX B");

    // multiple statements
    test_statements("NOP INX B LXI A, 0FFA6H");
    test_statements("LABEL: NOP INX B LXI A, 0FFA6H @LB1: NOP");
    test_statements("ALIAS: LABEL: NOP @LB1: NOP");

    // whitespace and comments
    test_statements("; comment");
    test_statements("LABEL: NOP ; comment\n INX B "
        "; comment\nLXI A,; comment\n 0FFA6H "
        "; comment\n @LB1:; comment\n NOP; comment\n");

    // nested instructions
    test_statements("LABEL: LXI A, (NOP)");
    test_statements("LABEL: LXI A, (LXI A , B)");

    // warnings
    test_statements("@LONGLABEL: NOP");
    test_statements("@LABEL: @SBPDF:");

    // errors
    // test_statements("1LABEL:");
    // test_statements("NOP:");
    // test_statements("LABEL: BLA");
    // test_statements("LABEL: LABEL:");
    // test_statements("Label: lAbel:");
    // test_statements("LABEL: LABEL");
}

int get_statements(char [], Statement []);

void test_statements(char *st)
{
    Statement *statements = (Statement *) calloc(MAX_STMNTS, sizeof(Statement));

    int nstmnt = get_statements(st, statements);

    for (int i = 0; i < nstmnt; i++) {
        
        // print program counter value
        printf("%04x\t", statements[i].pc);
        
        // print label
        if (statements[i].label)
            printf("%s:", statements[i].label);
        
        // print instruction
        if (statements[i].instr)
            printf("\t%s", statements[i].instr);

        // print arguments
        if (statements[i].args.nargs > 0) {
            putchar('\t');
            for (int j = 0; j < statements[i].args.ntok1; j++)
                printf("%s ", (statements[i].args.arg1+j)->str);
            if (statements[i].args.nargs == 2) {
                printf(", ");
                for (int j = 0; j < statements[i].args.ntok2; j++)
                    printf("%s ", (statements[i].args.arg2+j)->str);
            }
        }
        putchar('\n');
    }
    putchar('\n');
}