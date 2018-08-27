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

    // pseudo-instructions
    test_statements("oblab: DW 14H,1FH,16H");
    test_statements("oblab: DB 1415H,1F17H,16FFH");
    test_statements("oblab: DB 'abcdefg'");
    test_statements("oblab: ORG 14H");
    test_statements("oblab: name EQU 14H");
    test_statements("oblab: name SET val");
    test_statements("oblab: END");
    test_statements("oblab: IF 1 ENDIF");
    test_statements("oblab: name MACRO NOP ENDM");
    test_statements("oblab: name MACRO 1,2,3 NOP ENDM");

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
    // test_statements("oblab: SET val");
    // test_statements("oblab: EQU val");
    // test_statements("oblab: name");
    // test_statements("oblab: name NOP");
    
    // test_statements("Yo");
    // test_statements("what's up?");
    // test_statements("Yo what's up?");
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
        putchar('\t');

        // print name
        if (statements[i].name)
            printf("%s", statements[i].name);
        putchar('\t');
        
        // print instruction
        if (statements[i].instr)
            printf("%s", statements[i].instr);
        putchar('\t');

        // print arguments
        if (statements[i].args.nargs > 0) {
            for (int j = 0; j < statements[i].args.nargs; j++) {
                for (int k = 0; k < statements[i].args.args[j].ntoks; k++)
                    printf("%s ", statements[i].args.args[j].toks[k].str);
                if (j < statements[i].args.nargs-1)
                    printf(", ");
            }
        }
        putchar('\n');
    }
    putchar('\n');
}