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

int getword(char buf[], char word[], int maxlen);
void printerr(char *, ...);
Instr *getinstr(char *mnem, char *arg1, char *arg2);
char *validate_label(char *, const Statement[], int);

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

                // TODO: check arguments
                Instr *instr;
                if ((instr = getinstr(mnem, NULL, NULL)) == NULL) {
                    // TODO: check psuedo instructions
                    printerr("error: '%s' is not a defined mnemonic", mnem);
                    exit(1);
                }
                // store instruction
                if (statementp->instr == NULL)
                    statementp->instr = mnem;
                else
                    (++statementp)->instr = mnem;   // new statement
                // store and increment pc
                statementp->pc = pc;
                pc += instr->size;
            }
        } else {
            // skip comments
            if (*srcp == ';') {
                while (*srcp != '\n' && *srcp != '\0')
                    srcp++;
                if (*srcp == '\n')
                    srcp++;
                continue;
            }
            printerr("error: unexpected token '%c'", *srcp);
            exit(2);
        }
    }

    return statementp - statements +
        ((statementp->label || statementp->instr) ? 1 : 0);
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
        printerr("error: label '%s' is a defined mnemonic", label);
        exit(1);
    }

    // check for duplicate labels
    for (int i = 0; i < nstmnt; i++)
        if (statements[i].label && strcmp(statements[i].label, label) == 0) {
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
        if (strcmp(mnem, instrp->mnem) == 0)
            if (arg1 == NULL)
                return instrp;
            else if (strcmp(arg1, instrp->arg1) == 0)
                if (arg2 == NULL)
                    return instrp;
                else if (strcmp(arg2, instrp->arg2) == 0)
                    return instrp;
        instrp++;
    }
    return NULL;
}