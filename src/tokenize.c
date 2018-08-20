// tokenize.c: Assembler lexical analysis, syntax checking
//  and program statement collection
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "asm.h"

#define iswdchr(c) (isalnum(c) || (c) == '@' || (c) == '?')

int get_argument(char *, Expr *);
int getword(char *, char *, int);
int gettoktype(char *);
char *validate_label(char *, const Statement[], int);
Instr *getinstr(char *, char *, char *);
Oper *getunaoper(char *);
Oper *getbinoper(char *);
void printerr(char *, ...);
int strcicmp(const char *, const char *);

// get_statements: collects program statements and checks for syntax errors,
//   returns total number of collected statements.
int get_statements(char srcbuf[], Statement statements[])
{
    char *srcp = srcbuf;                // pointer in srcbuf
    Statement *statementp = statements; // pointer in statements
    uint16_t pc;                        // program counter

    pc = 0;
    while (statementp - statements < MAX_STMNTS && *srcp != '\0') {

        // skip whitespace
        while (isspace(*srcp))
            srcp++;

        // collect a word
        char word[WORD_LEN+1];
        int wlen;
        srcp += wlen = getword(srcp, word, WORD_LEN);

        if (wlen > 0) {
            if (*srcp == ':') {
                // end any open statement
                if (statementp->label || statementp->instr)
                    statementp++;   // start a new statement

                // validate and copy the label
                char *label = validate_label(word, statements,
                    statementp-statements);

                // store label in the new statement
                statementp->label = label;

                // store pc
                statementp->pc = pc;
                srcp++;
            } else {
                char *mnem = (char *) malloc(wlen+1);
                strcpy(mnem, word);

                Instr *insinfo;
                if ((insinfo = getinstr(mnem, NULL, NULL)) == NULL) {
                    // TODO: check psuedo instructions
                    printerr("error: '%s' is not a defined mnemonic", mnem);
                    exit(1);
                }

                // store instruction
                if (statementp->instr == NULL)
                    statementp->instr = mnem;
                else
                    (++statementp)->instr = mnem;   // new statement

                // collect arguments
                if (insinfo->nargs > 0) {
                    int alen;

                    // collect first argument
                    Expr *arg1 = (Expr *) malloc(sizeof(Expr));
                    srcp += alen = get_argument(srcp, arg1);
                    if (alen == 0) {
                        printerr("error: instruction %s takes %d arguments. "
                            "None provided.", mnem, insinfo->nargs);
                        exit(1);
                    } else {
                        statementp->arg1 = arg1;
                        if (insinfo->nargs == 2) {
                            // skip whitespace
                            while (isspace(*srcp))
                                srcp++;
                            // collect comma
                            if (*srcp != ',') {
                                if (iswdchr(*srcp))
                                    printerr("error: instruction %s takes %d "
                                        "arguments. 1 provided.",
                                        mnem, insinfo->nargs);
                                else
                                    printerr("error: unexpected token '%c'", *srcp);
                                exit(1);
                            } else
                                srcp++;
                            // collect second argument
                            Expr *arg2 = (Expr *) malloc(sizeof(Expr));
                            srcp += alen = get_argument(srcp, arg2);
                            if (alen == 0) {
                                printerr("error: instruction %s takes %d arguments. "
                                    "1 provided.", mnem, insinfo->nargs);
                                exit(1);
                            } else
                                statementp->arg2 = arg2;
                        }
                    }
                }

                // store and increment pc
                statementp->pc = pc;
                pc += insinfo->size;
            }
        } else if (*srcp == ';') {
            // skip comments
            while (*srcp != '\n' && *srcp != '\0')
                srcp++;
            if (*srcp == '\n')
                srcp++;
            continue;
        } else if (*srcp != '\0') {
            printerr("error: unexpected token '%c'", *srcp);
            exit(1);
        }
    }

    return statementp - statements +
        ((statementp->label || statementp->instr) ? 1 : 0);
}

// get_argument: collect an instruction argument expression and return the number of
//  characteres read from the buffer.
int get_argument(char buf[], Expr *exp)
{
    char *bufp;         // pointer in buf

    // skip whitespace
    while (isspace(*bufp))
        bufp++;

    // collect word
    char word[WORD_LEN+1];
    int wlen;

    bufp += wlen = getword(bufp, word, WORD_LEN);
    if (wlen > 0) {
        // TODO: store type and value of first operand
    } else {
        if (*bufp == '(') {
            // TODO: collect a new expression until ')'
        }
    }
    // TODO: unary and binary operators, second operand.

    return bufp-buf;
}

// getword: gets a word from a character buffer and returns the length of the word.
int getword(char buf[], char word[], int maxlen)
{
    char *bufp = buf;   // pointer in buf
    char *wordp = word; // pointer in word

    while (iswdchr(*bufp) && maxlen-- > 0 && *buf != '\0')
        *wordp++ = *bufp++;
    *wordp = '\0';
    return wordp-word;
}

// gettoktype: returns type of word as one of the TokType values
int gettoktype(char word[])
{
    int wlen = strlen(word);
    char *wordp = word;

    if (isdigit(*wordp)) {
        // numbers
        char base = tolower(word[wlen-1]);
        int islastdig = isdigit(base) ? 1 : 0;
        if (islastdig)
            base = 'd';

        switch (base) {
            case 'b':   // binary
                while (wordp-word < wlen-1) {
                    if (*wordp-'0' < 2)
                        wordp++;
                    else {
                        printerr("error: binary number %s cannot contain %c",
                            word, *wordp);
                        exit(1);
                    }
                }
                return TOK_BIN;
            case 'o': case 'q': // octal
                while (wordp-word < wlen-1) {
                    if (*wordp-'0' < 8)
                        wordp++;
                    else {
                        printerr("error: octal number %s cannot contain %c",
                            word, *wordp);
                        exit(1);
                    }
                }
                return TOK_OCT;
            case 'd':   // decimal
                while (wordp-word < wlen-1+islastdig) {
                    if (isdigit(*wordp))
                        wordp++;
                    else {
                        printerr("error: decimal number %s cannot contain %c",
                            word, *wordp);
                        exit(1);
                    }
                }
                return TOK_DEC;
            case 'h':   // hexadecimal
                while (wordp-word < wlen-1) {
                    if (isxdigit(*wordp))
                        wordp++;
                    else {
                        printerr("error: hexadecimal number %s cannot contain %c",
                            word, *wordp);
                        exit(1);
                    }
                }
                return TOK_HEX;
        }
    } else if (getinstr(word, NULL, NULL))  // instruction
        return TOK_INSTR;
    // TODO: pseudo-instructions
    else if (getunaoper(word))              // unary operator
        return TOK_UNAOPER;
    else if (getbinoper(word))              // binary operator
        return TOK_BINOPER;
    else                                    // label
        return TOK_LABEL;
}

// validate_label: throw errors if word is not a valid label and copy at most 5
//  characters from label into a safe place.
char *validate_label(char *word, const Statement statements[], int nstmnt)
{
    char *label = (char *) malloc(LABEL_LEN+1);
    strncpy(label, word, LABEL_LEN);

    // validate the label
    if (isdigit(label[0])) {
        printerr("error: the first character in label '%s' cannot "
            "be a digit", label);
        exit(1);
    }

    // TODO: check if label is pseudo-instruction
    // check if label is instruction
    if (getinstr(label, NULL, NULL) != NULL) {
        printerr("error: label %s is a defined mnemonic", label);
        exit(1);
    }

    // check for duplicate labels
    for (int i = 0; i < nstmnt; i++)
        if (statements[i].label && strcicmp(statements[i].label, label) == 0) {
            printerr("error: label '%s' is a duplicate", label);
            exit(1);
        }

    // give warning about short label
    if (strlen(word) > LABEL_LEN)
        printerr("warning: the label '%s' is too long; "
            "using '%s' instead", word, label);
    return label;
}

extern Instr instrs[];

// getinstr: returns the first instruction definition with the matching criteria
//  or NULL if it doesn't exist.
Instr *getinstr(char *mnem, char *arg1, char *arg2)
{
    Instr *instrp = instrs;
    while (instrp - instrs < 0x100 && instrp->mnem) {
        if (strcicmp(mnem, instrp->mnem) == 0)
            if (arg1 == NULL)
                return instrp;
            else if (strcicmp(arg1, instrp->arg1) == 0)
                if (arg2 == NULL)
                    return instrp;
                else if (strcicmp(arg2, instrp->arg2) == 0)
                    return instrp;
        instrp++;
    }
    return NULL;
}

extern Oper unaopers[];
extern const int N_UNAOPERS;

// getunaoper: returns the unary operator definition with matching string
Oper *getunaoper(char *s)
{
    Oper *operp = unaopers;
    while (operp - unaopers < N_UNAOPERS) {
        if (strcicmp(s, operp->str) == 0)
            return operp;
        operp++;
    }
    return NULL;
}

extern Oper binopers[];
extern const int N_BINOPERS;

// getbinoper: returns the binary operator definition with matching string
Oper *getbinoper(char *s)
{
    Oper *operp = binopers;
    while (operp - binopers < N_BINOPERS) {
        if (strcicmp(s, operp->str) == 0)
            return operp;
        operp++;
    }
    return NULL;
}

// strcicmp: case insensitive string comparison
int strcicmp(const char *s, const char *t)
{
    for ( ; *s && *t; s++, t++) {
        int d = tolower(*s) - tolower(*t);
        if (d != 0)
            return d;
    }
    if (!*s && !*t)
        return 0;
    if (*t)
        return -1;
    return 1;
}