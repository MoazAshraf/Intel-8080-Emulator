#include <stdio.h>  // TODO: remove this
#include <stdlib.h>
#include "asm.h"

int evaluate(const Arg *, const Label [], int, int);
int str_to_int(char *, int);
int ascii_to_code(char *);

extern Oper binopers[];
extern Oper unaopers[];
extern Pseudo pseudos[];

// assemble: assemble an array of statements into Intel 8080 machine code,
//  return length of output.
int assemble(const Statement statements[], int nstmnt, uint8_t outbuf[])
{
    int pc;                     // program counter value
    uint8_t *outp;              // pointer in outbuf
    Label labels[MAX_STMNTS];   // instruction labels
    int nlabels;
    int i, j;
    int progsize;               // program size

    // collect instruction labels
    for (i = 0, j = 0; i < nstmnt; i++) { 
        if (statements[i].label) {
            labels[j].key = statements[i].label;
            labels[j++].value = statements[i].pc;
        }
    }
    nlabels = j;
    outp = outbuf;
    progsize = 0;

    // iterate over all statements and assemble
    for (i = 0; i < nstmnt; i++) {
        // store program counter value
        pc = statements[i].pc;

        // evaluate arguments
        int nargs = statements[i].args.nargs;
        unsigned int args[nargs];

        for (j = 0; j < nargs; j++) {
            args[j] = evaluate(statements[i].args.args+j, labels, nlabels, pc);
            // printf("%d\n", args[j]);    // TODO: remove this
        }

        // convert to machine code
        if (statements[i].instr) {
            
            Instr *instr;
            Pseudo *pseudo;
            int arg1 = (nargs > 0) ? args[0] : -1;
            int arg2 = (nargs > 1) ? args[1] : -1;
            
            if (instr = get_instr(statements[i].instr, arg1, arg2)) {
                // instructions
                if (outp-outbuf+instr->size >= MAX_PROG) {
                    printerr("error: program size limit reached");
                    exit(EXIT_FAILURE);
                } else {
                    // add opcode
                    *outp++ = instr->opcode;

                    // argument data
                    unsigned int data;
                    if (instr->arg1 == -1)
                        data = args[0];
                    else if (instr->arg2 == -1)
                        data = args[1];

                    if (instr->size == 2) {
                        // add 1 byte argument data
                        if (data > 0xff) {
                            printerr("error: provided data argument for %s exceeds "
                                "1 byte", instr->mnem);
                            exit(EXIT_FAILURE);
                        } else
                            *outp++ = data & 0xff;
                    } else if (instr->size == 3) {
                        // add 2 byte argument data
                        if (data > 0xffff) {
                            printerr("error: provided data argument for %s exceeds "
                                "2 bytes", instr->mnem);
                            exit(EXIT_FAILURE);
                        } else {
                            uint8_t lowdat = data & 0xff;
                            uint8_t hidat = (data & 0xff00) >> 8;
                            *outp++ = lowdat;
                            *outp++ = hidat;
                        }
                    }
                }
            } else if (pseudo = get_pseudo(statements[i].instr)) {
                // pseudo-instructions
                
                if (pseudo->nargs < 3 && nargs != pseudo->nargs) {
                    printerr("error: %s requires %d arguments. "
                        "%d provided", pseudo->mnem, pseudo->nargs, nargs);
                    exit(EXIT_FAILURE);
                } else if (pseudo->nargs == 3 && nargs == 0) {
                    printerr("error: %s takes a list of arguments. "
                        "no arguments provided", pseudo->mnem);
                    exit(EXIT_FAILURE);
                }

                if (pseudo == pseudos+0) {          // DB: Define bytes
                    for (j = 0; j < nargs; j++) {
                        if (args[j] > 0xff) {
                            printerr("error: provided data argument for DB "
                                "exceeds 1 bytes");
                            exit(EXIT_FAILURE);
                        }
                        *outp++ = args[j];
                    }
                } else if (pseudo == pseudos+1) {   // DW: Define words
                    for (j = 0; j < nargs; j++) {
                        if (args[j] > 0xffff) {
                            printerr("error: provided data argument for DW "
                                "exceeds 2 bytes");
                            exit(EXIT_FAILURE);
                        }
                        uint8_t lowdat = args[j] & 0xff;
                        uint8_t hidat = (args[j] & 0xff00) >> 8;
                        *outp++ = lowdat;
                        *outp++ = hidat;
                    }
                } else if (pseudo == pseudos+2) {   // DS: Define storage
                    if (outp-outbuf+args[0] >= MAX_PROG) {
                        printerr("error: not enough program memory for DS");
                        exit(EXIT_FAILURE);
                    }
                    outp += args[0];
                } else if (pseudo == pseudos+3) {   // ORG: Origin
                    if (args[0] < MAX_PROG)
                        outp = outbuf + args[0];
                } else if (pseudo == pseudos+4) {   // TODO: EQU
                    
                } else if (pseudo == pseudos+5) {   // TODO: SET
                    
                } else if (pseudo == pseudos+6) {   // END: End of assembly
                    break;
                } else if (pseudo == pseudos+7) {   // TODO: IF
                    
                } else if (pseudo == pseudos+8) {   // TODO: ENDIF
                    
                } else if (pseudo == pseudos+9) {   // TODO: MACRO
                    
                } else if (pseudo == pseudos+10) {  // TODO: ENDM
                    
                }

            } else {
                printerr("error: invalid instruction");
                exit(EXIT_FAILURE);
            }

            if (outp-outbuf > progsize)
                progsize = outp-outbuf;
        }

        // TODO: execute pseudo-instructions
        // TODO: macros
        // TODO: assemble instructions and data in outbuf
    }
    
    return progsize;
}

int eval_list(ExprNode *);

// evaluate: evaluates an argument expression to an integer value.
int evaluate(const Arg *arg, const Label labels[], int nlabels, int pc)
{
    static int i = 0;

    // expression linked list
    ExprNode *head = NULL;
    ExprNode *curr = NULL;

    int result = 0;
    int inparen = 0;

    // collect expression nodes
    for ( ; i < arg->ntoks; i++) {
        int val;

        // allocate memory for nodes
        if (curr == NULL) {
            curr = (ExprNode *) malloc(sizeof(ExprNode));
            curr->prev = NULL;
            head = curr;
        } else if (curr->next == NULL) {
            curr->next = (ExprNode *) malloc(sizeof(ExprNode));
            curr->next->prev = curr;
            curr = curr->next;
        }
        curr->next = NULL;

        // evaluate parentheses
        if (arg->toks[i].str[0] == '(') {
            if (i+1 < arg->ntoks) {
                i++;
                if (arg->toks[i].type == TOK_INSTR) {
                    // TODO: assemble nested instruction
                    // if (i+1 < arg->ntoks)    // collect arguments
                } else {    // evaluate parentheses
                    val = evaluate(arg, labels, nlabels, pc);
                    if (arg->toks[i].str[0] == ')') {
                        curr->value = val;
                        curr->oper = NULL;
                        curr->type = ENT_VAL;
                    } else {
                        printerr("error: missing closing )");  // TODO: remove this
                        exit(EXIT_FAILURE);
                    }
                }
            }
        } else if (arg->toks[i].str[0] == ')') {
            if (curr->prev)
                curr->prev->next = NULL;
            free(curr);
            curr = NULL;
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
                    // TODO: ASCII lists
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
            curr->oper == NULL;
            curr->type = ENT_VAL;
        } else if (arg->toks[i].type == TOK_UNAOPER) {
            curr->oper = get_unaoper(arg->toks[i].str);
            curr->type = ENT_UNA;
        } else if (arg->toks[i].type == TOK_BINOPER) {
            curr->oper = get_binoper(arg->toks[i].str);
            curr->type = ENT_BIN;
        }
    }

    // evaluate expression list
    result = eval_list(head);

    if (!inparen)
        i = 0;
    return result;
}

int eval_unaoper(Oper *, int);
int eval_binoper(Oper *, int, int);

// eval_list: evaluate an expression list and return the result
int eval_list(ExprNode *head)
{
    ExprNode *curr;

    // while the head is not the only node
    while (head->next) {
        // go through the list
        for (curr = head; curr; curr = curr->next) {
            // if node is operand
            if (curr->type == ENT_VAL) {
                Oper *prev_oper = NULL;
                Oper *next_oper = NULL;

                // get previous operator
                if (curr->prev)
                    if (curr->prev->oper)
                        prev_oper = curr->prev->oper;
                    else {
                        printerr("error: expected operator before operand in "
                            "expression list");
                        exit(EXIT_FAILURE);
                    }
                // get next operator
                if (curr->next)
                    if (curr->next->oper)
                        next_oper = curr->next->oper;
                    else {
                        printerr("error: expected operator after operand in "
                            "expression list");
                        exit(EXIT_FAILURE);
                    }
                // if left operator has higher or equal precedence than right operator
                //  or if operand is the last node
                if (prev_oper)
                    if (!curr->next || next_oper && prev_oper->prec <= next_oper->prec) {
                        // evaluate left operator
                        if (curr->prev->type == ENT_UNA) {  // unary operator
                            int val = eval_unaoper(prev_oper, curr->value);
                            curr->prev->value = val;
                            curr->prev->type = ENT_VAL;
                            curr->prev->oper = NULL;
                            curr->prev->next = curr->next;
                            if (curr->next)
                                curr->next->prev = curr->prev;
                        } else if (curr->prev->type == ENT_BIN) {   // binary operator
                            if (curr->prev->prev
                              && curr->prev->prev->type == ENT_VAL) {
                                int val = eval_binoper(prev_oper,
                                    curr->prev->prev->value, curr->value);
                                curr->prev->prev->value = val;
                                curr->prev->prev->next = curr->next;
                                if (curr->next)
                                    curr->next->prev = curr->prev->prev;
                            } else {
                                printerr("error: expected operand before operator in "
                                    "expression list");
                                exit(EXIT_FAILURE);
                            }
                        }
                        break;
                    }
            }
        }
    }

    return head->value;
}

// eval_unaoper: evaluate an operation with one operand.
int eval_unaoper(Oper *oper, int op)
{
    if (oper == unaopers+0)         // + (positive sign)
        return op;
    else if (oper == unaopers+1)    // - (negative sign)
        return -op;
    else if (oper == unaopers+2)    // NOT (logical NOT)
        return ~op & 0xffff;
}

// eval_binoper: evaluate an operation with two operands.
int eval_binoper(Oper *oper, int op1, int op2)
{
    if (oper == binopers+0)         // * (multiply)
        return op1 * op2;
    else if (oper == binopers+1)    // / (divide)
        return op1 / op2;
    else if (oper == binopers+2)    // MOD (modulo)
        return op1 % op2;
    else if (oper == binopers+3)    // SHR (shift right)
        return op1 >> op2;
    else if (oper == binopers+4)    // SHL (shift left)
        return op1 << op2;
    else if (oper == binopers+5)    // + (addition)
        return op1 + op2;
    else if (oper == binopers+6)    // + (subtraction)
        return op1 - op2;
    else if (oper == binopers+7)    // AND (bitwise AND)
        return op1 & op2;
    else if (oper == binopers+8)    // OR (bitwise OR)
        return op1 | op2;
    else if (oper == binopers+9)    // XOR (bitwise XOR)
        return op1 ^ op2;
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
            dig = 10 + *s-'A';
        else if (islower(*s))
            dig = 10 + *s-'a';
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