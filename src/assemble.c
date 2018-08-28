#include <stdlib.h>
#include "asm.h"

int evaluate(const Arg *, const Label [], int);
int str_to_int(char *, int);

// assemble: assemble an array of statements into Intel 8080 machine code,
//  return length of output.
int assemble(const Statement statements[], int nstmnt, char outbuf[])
{
    int pc;                     // program counter value
    char *outp;                 // pointer in outbuf
    Label labels[MAX_STMNTS];   // instruction labels
    int i, j;

    // collect instruction labels
    for (i = 0, j = 0; i < nstmnt; i++) { 
        if (statements[i].label) {
            labels[j].key = statements[i].label;
            labels[j++].value = statements[i].pc;
        }
    }

    // iterate over all statements and assemble
    for (i = 0; i < nstmnt; i++) {
        // store program counter value
        pc = statements[i].pc;

        // TODO: evaluate arguments
        // TODO: check arguments count and size
        // TODO: execute pseudo-instructions
        // TODO: macros
        // TODO: assemble instructions and data in outbuf
    }

    return outp-outbuf;
}

// evaluate: evaluates an argument expression to an integer value.
int evaluate(const Arg *arg, const Label labels[], int pc)
{
    static int i = 0;

    // expression linked list
    ExprNode *head = (ExprNode *) malloc(sizeof(ExprNode));
    ExprNode *curr = head;

    int result = 0;
    int inparen = 0;

    // collect expression nodes
    for (; i < arg->ntoks; i++)
        if (arg->toks[i].str[0] == '(' && i+1 < arg->ntoks) {
            // evaluate parentheses
            i++;
            int val = evaluate(arg, labels, pc);
            if (arg->toks[i].str[0] == ')') {
                curr->value = val;
                curr->next = (ExprNode *) malloc(sizeof(ExprNode));
                curr = curr->next;
                continue;
            }
        } else if (arg->toks[i].str[0] == ')') {
            inparen = 1;
            break;
        }
        else if (isoperand(arg->toks[i].type)) {
            // evaluate operands
            int val;
            switch (arg->toks[i].type) {
                case TOK_HEX:
                    val = str_to_int(arg->toks[i].str, 16);
                    break;
                case TOK_DEC:
                    val = str_to_int(arg->toks[i].str, 10);
                    break;
                case TOK_OCT:
                    val = str_to_int(arg->toks[i].str, 8);
                    break;
                case TOK_BIN:
                    val = str_to_int(arg->toks[i].str, 2);
                    break;
                case TOK_PC:
                    val = pc;
                    break;
                // TODO: ASCII
                // TODO: labels
            }
        }
        // TODO: operators

    // TODO: evaluate expression list

    if (!inparen)
        i = 0;
    return result;
}

// str_to_int: converts a number string formatted with the specified base
//  to an integer.
int str_to_int(char *s, int b)
{
    // TODO: implement this function
    return 0;
}