// test_arg.c: tests for get_token() and get_argument() in tokenize.c
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdio.h>
#include <stdlib.h>
#include "../src/asm.h"

char *prog;

void test_tokens(char *);

int main()
{
    prog = "test";

    // numbers
    test_tokens("328");
    test_tokens("0FFH");
    test_tokens("0101B");
    test_tokens("10010b");
    test_tokens("777o");
    test_tokens("777Q");

    // ascii
    test_tokens("'O'");
    test_tokens("'?'");
    
    // operators
    test_tokens("*");
    test_tokens("-");
    test_tokens("+");
    test_tokens("/");
    test_tokens("MOD");
    test_tokens("AND");
    test_tokens("or");
    test_tokens("not");
    test_tokens("SHR");
    test_tokens("shl");
    test_tokens("xor");

    // parenthesis
    test_tokens("(");
    test_tokens(")");

    // labels
    test_tokens("label");
    test_tokens("LABEL");
    
    // instructions
    test_tokens("NOP");
    test_tokens("LXI");

    // program counter
    test_tokens("$");

    // skipped stuff
    test_tokens(" ; comments and stuff \n ; comments ");
    test_tokens(",");
    test_tokens("");

    // expressions
    test_tokens("5 + 2");
    test_tokens("05H - 23O");
    test_tokens("-54");
    test_tokens("+0101B");
    test_tokens("NOT LABEL");
    test_tokens("(2+3)*6-4*9");
    test_tokens("'A' + 'B'");

    // illegal stuff
    // test_tokens(".");
    // test_tokens("800O");
    // test_tokens("FFH");
    // test_tokens("ttH");
    // test_tokens("2b");
    // test_tokens("'?");
    // test_tokens("?'");

    return 0;
}

int get_token(char *, Token *);

// test_tokens: collects all tokens in s prints them and their types
void test_tokens(char *s)
{
    Token tok;
    int len;

    while ((len = get_token(s, &tok)) > 0 && tok.type != -1) {
        s += len;
        printf("%s\n", tok.str);
        char *type;

        switch (tok.type) {
            case TOK_HEX:
                type = "Hex";
                break;
            case TOK_DEC:
                type = "Dec";
                break;
            case TOK_OCT:
                type = "Oct";
                break;
            case TOK_BIN:
                type = "Bin";
                break;
            case TOK_PC:
                type = "Program counter";
                break;
            case TOK_ASCII:
                type = "ASCII Literal";
                break;
            case TOK_LABEL:
                type = "Label";
                break;
            case TOK_INSTR:
                type = "Instruction";
                break;
            case TOK_UNAOPER:
                type = "Unary operator";
                break;
            case TOK_BINOPER:
                type = "Binary operator";
                break;
            case TOK_PAREN:
                type = "Parenthesis";
                break;
            default:
                type = "Unknown";
                break;
        }

        free(tok.str);
        printf("TYPE: %s\n\n", type);
    }
}