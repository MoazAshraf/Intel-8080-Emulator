// test_arg.c: tests for get_argument() in tokenize.c
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/asm.h"

char *prog;

void test_argument(char *);

int main()
{
    prog = "test";

    // single operands
    test_argument("0FFH");
    test_argument("124");
    test_argument("777o");
    test_argument("010b");
    test_argument("$");
    test_argument("'a'");
    test_argument("LABEL");

    // expressions
    test_argument("(45)");
    test_argument("25D + 2FH");
    test_argument("3 - 1 * 4 + 8");
    test_argument("(3 - 1) * (4 + 8)");
    test_argument("((3 - 1) * (4 + 8))");
    test_argument("(((3 - 1) * (4 + 8)))");
    test_argument("$+10FDH");
    test_argument("+32");
    test_argument("-32");
    test_argument("---++-+32");
    test_argument("+32");
    test_argument("-32");
    test_argument("+-42---++-+32");

    // termination
    test_argument("45 NOP");
    test_argument("45, 32");
    test_argument("NOP");

    // nested instructions
    test_argument("(NOP)");
    test_argument("(INX B)");
    test_argument("(LXI B, 0F6AH)");
    test_argument("(NOP) + (INX B) - 4");
    test_argument("(LXI B, (LXI B, (LXI B, 0F6AH)))");

    // errors
    // test_argument("(");
    // test_argument("+");
    // test_argument("-");
    // test_argument("+4*");
    // test_argument("*-");
    // test_argument("*");
    // test_argument("4 5");
    // test_argument("(4 + 5) 5");
    // test_argument("(3 - 1) * (4 + 8");

    return 0;
}

int get_argument(char *, Token **, int *);

void test_argument(char *s)
{
    Token *arg;
    Token *tokp;
    int ntoks;
    get_argument(s, &arg, &ntoks);

    for (tokp = arg; tokp-arg < ntoks; tokp++) {
        if (tokp->type == TOK_BINOPER)
            printf("  %s   ", tokp->str);
        else
            printf("%s ", tokp->str);
        
        if (tokp-arg == ntoks-1)
            putchar('\n');
    }
}