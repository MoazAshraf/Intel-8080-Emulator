// test_statements.c: tests for get_argument() in tokenize.c
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdio.h>
#include <stdlib.h>
#include "../src/asm.h"

char *prog;

int getoptype(char *);
void test_argument(char *);

int main()
{
    prog = "test";

    printf("%d\n", getoptype("10F1H"));
    printf("%d\n", getoptype("342"));
    printf("%d\n", getoptype("349D"));
    printf("%d\n", getoptype("173O"));
    printf("%d\n", getoptype("173Q"));
    printf("%d\n", getoptype("01010B"));
    // test_argument("5");
    // test_argument("5 + 3");
    // test_argument(" 5 * 5 - 2 ");
    // test_argument(" 2 + 8 / 5 ");
}

int get_argument(char buf[], Expr *arg);
void printexp(Expr *e);

void test_argument(char *s)
{
    Expr *arg = (Expr *) malloc(sizeof(Expr));
    get_argument(s, arg);

    // print argument expression
    printexp(arg);
    putchar('\n');
}

void printexp(Expr *e)
{
    if (e->value)
        printf("%s", e->value);
    else {
        if (e->op1) {
            putchar('(');
            printexp(e->op1);
            putchar(')');
            if (e->op2) {
                if (e->oper)
                    printf(" %s ", e->oper);
                putchar('(');
                printexp(e->op2);
                putchar(')');
            }
        }
    }
}