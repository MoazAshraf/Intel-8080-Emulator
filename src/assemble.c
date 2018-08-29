#include <stdlib.h>
#include "asm.h"

int evaluate(const Arg *, const Label [], int, int);
int str_to_int(char *, int);
int ascii_to_code(char *);

// assemble: assemble an array of statements into Intel 8080 machine code,
//  return length of output.
int assemble(const Statement statements[], int nstmnt, char outbuf[])
{
    int pc;                     // program counter value
    char *outp;                 // pointer in outbuf
    Label labels[MAX_STMNTS];   // instruction labels
    int nlabels;
    int i, j;

    // collect instruction labels
    for (i = 0, j = 0; i < nstmnt; i++) { 
        if (statements[i].label) {
            labels[j].key = statements[i].label;
            labels[j++].value = statements[i].pc;
        }
    }
    nlabels = j;

    outp = outbuf;

    // iterate over all statements and assemble
    for (i = 0; i < nstmnt; i++) {
        // store program counter value
        pc = statements[i].pc;

        // evaluate arguments
        int args[statements[i].args.nargs];

        for (j = 0; j < statements[i].args.nargs; j++)
            args[j] = evaluate(statements[i].args.args+j, labels, nlabels, pc);

        // check arguments count and size
        // TODO: execute pseudo-instructions
        // TODO: macros
        // TODO: assemble instructions and data in outbuf
    }
    
    return outp-outbuf;
}

// evaluate: evaluates an argument expression to an integer value.
int evaluate(const Arg *arg, const Label labels[], int nlabels, int pc)
{
    static int i = 0;

    // expression linked list
    ExprNode *head = (ExprNode *) malloc(sizeof(ExprNode));
    ExprNode *curr = head;

    int result = 0;
    int inparen = 0;

    // collect expression nodes
    for (; i < arg->ntoks; i++) {
        int val;

        if (arg->toks[i].str[0] == '(') {
            if (i+1 < arg->ntoks) {
                i++;
                if (arg->toks[i].type == TOK_INSTR) {
                    // TODO: assemble nested instruction
                    // if (i+1 < arg->ntoks)    // collect arguments
                } else {    // evaluate parentheses
                    val = evaluate(arg, labels, nlabels, pc);
                    if (arg->toks[i].str[0] == ')')
                        curr->value = val;
                    else
                        printerr("debug warning: ) missing.");  // TODO: remove this
                }
            }
        } else if (arg->toks[i].str[0] == ')') {
            inparen = 1;
            break;
        } else if (isoperand(arg->toks[i].type)) { // evaluate operands
            int j;

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
                case TOK_ASCII:
                    val = ascii_to_code(arg->toks[i].str);
                    break;
                case TOK_LABEL:
                    for (j = 0; j < nlabels; j++)
                        if (strcicmp(labels[j].key, arg->toks[i].str) == 0) {
                            val = labels[j].value;
                            break;
                        }
                    break;
            }
            curr->value = val;
        } else if (arg->toks[i].type == TOK_UNAOPER)
            curr->oper = get_unaoper(arg->toks[i].str);
        else if (arg->toks[i].type == TOK_BINOPER)
            curr->oper = get_binoper(arg->toks[i].str);

        curr->next = (ExprNode *) malloc(sizeof(ExprNode));
        curr = curr->next;

        // TODO: operators
    }

    // TODO: evaluate expression list

    if (!inparen)
        i = 0;
    return result;
}

#include <ctype.h>

// str_to_int: converts a number string formatted with the specified base
//  to an integer.
int str_to_int(char *s, int b)
{
    int result = 0;

    for ( ; *s; s++) {
        char dig;
        if (isdigit(*s))
            dig = *s-'0';
        else if (isupper(*s))
            dig = *s-'A';
        else if (islower(*s))
            dig = *s-'a';
        result = result * b + dig;
    }

    return result;
}

// ascii_to_code: converts an ASCII literal into its equivalent integer code
//  (hex values concatenated).
int ascii_to_code(char *s)
{
    int result = 0;

    for ( ; *s; s++)
        result = 16 * result + *s;
    
    return result;
}