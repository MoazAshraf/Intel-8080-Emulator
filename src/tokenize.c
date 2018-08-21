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

int get_argument(char *, Token *);
int get_token(char *, Token *);
int getword(char *, char *, int);
Instr *get_instr(char *, char *, char *);
Oper *get_unaoper(char *);
Oper *get_binoper(char *);
char *validate_label(char *, const Statement[], int);
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
        char word[MAX_WORD+1];
        int wlen;
        srcp += wlen = getword(srcp, word, MAX_WORD);

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
                if ((insinfo = get_instr(mnem, NULL, NULL)) == NULL) {
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
                    Token *arg1;
                    srcp += alen = get_argument(srcp, arg1);
                    if (alen == 0) {
                        printerr("error: instruction %s takes %d arguments. "
                            "None provided.", mnem, insinfo->nargs);
                        exit(1);
                    } else {
                        statementp->arg1 = arg1;
                        if (insinfo->nargs == 2) {
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
                            Token *arg2;
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

// get_argument: collects an argument expression, returns #characters read.
int get_argument(char buf[], Token *arg)
{
    // TODO: allocate memory for arg
    // TODO: collect tokens in arg
    // TODO: unary and binary + and - type check
    // TODO: syntax check
    // TODO: return #characters read
}

// get_token: collects an argument expression token, returns #characters read.
int get_token(char buf[], Token *tok)
{
    char *bufp; // pointer in buf
    char word[MAX_WORD+1];
    char *wordp;
    int wlen;

    bufp = buf;

    // skip whitespace
    while (isspace(*bufp))
        bufp++;
    // skip comments
    while (*bufp == ';') {
        while (*bufp != '\n' && *bufp != '\0')
            bufp++;
        if (*bufp == '\n')
            bufp++;
        // skip whitespace
        while (isspace(*bufp))
        bufp++;
    }

    tok->type = -1;

    // collect a word
    bufp += wlen = getword(bufp, word, MAX_WORD);
    wordp = word;

    if (wlen > 0) { // if word collected
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
                    tok->type = TOK_BIN;
                    break;
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
                    tok->type = TOK_OCT;
                    break;
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
                    tok->type = TOK_DEC;
                    break;
                case 'h':   // hexadecimal
                    while (wordp-word < wlen-1) {
                        if (isxdigit(*wordp))
                            wordp++;
                        else {
                            printerr("error: hexadecimal number %s cannot "
                                "contain %c", word, *wordp);
                            exit(1);
                        }
                    }
                    tok->type = TOK_HEX;
                    break;
                default:
                    printerr("error: illegal token %s", word);
                    exit(1);
            }
        } else if (get_instr(word, NULL, NULL)) // instruction
            tok->type = TOK_INSTR;
        // TODO: pseudo-instructions
        else if (get_binoper(word))             // binary operator
            tok->type = TOK_BINOPER;
        else if (get_unaoper(word))             // unary operator
            tok->type = TOK_UNAOPER;
        else                                    // label
            tok->type = TOK_LABEL;
    } else if (*bufp) {    // non-word character
        *wordp++ = *bufp++;
        *wordp = '\0';

        if (*word == '(' || *word == ')')       // parentheses
            tok->type = TOK_PAREN;
        else if (*word == '$')                  // program counter
            tok->type = TOK_PC;
        else if (get_binoper(word))             // binary operator
            tok->type = TOK_BINOPER;
        else if (get_unaoper(word))             // unary operator
            tok->type = TOK_UNAOPER;
        else if (*word == '\'') {               // ascii constant
            wordp = word;
            while (*bufp && *bufp != '\'')
                *wordp++ = *bufp++;
            if (*bufp == '\'')
                bufp++;
            else {
                printerr("error: missing ASCII delimiter");
                exit(1);
            }
            tok->type = TOK_ASCII;
        }
        else if (*word == ',') {
            bufp--;
            *word = '\0';
            tok->type = -1;
        } else if (!*word) {
            *word = '\0';
            tok->type = -1;
        } else {
            printerr("error: illegal token %s", word);
            exit(1);
        }
    }

    // copy token string
    wlen = strlen(word);
    if (wlen > 0) {
        tok->str = (char *) malloc(wlen+1);
        strcpy(tok->str, word);
    }

    return bufp-buf;
}

// getword: gets a word from a character buffer, returns the length of the word.
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
    char *label = (char *) malloc(MAX_LABEL+1);
    strncpy(label, word, MAX_LABEL);

    // validate the label
    if (isdigit(label[0])) {
        printerr("error: the first character in label '%s' cannot "
            "be a digit", label);
        exit(1);
    }

    // TODO: check if label is pseudo-instruction
    // check if label is instruction
    if (get_instr(label, NULL, NULL) != NULL) {
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
    if (strlen(word) > MAX_LABEL)
        printerr("warning: the label '%s' is too long; "
            "using '%s' instead", word, label);
    return label;
}

extern Instr instrs[];

// get_instr: returns the first instruction definition with the matching criteria
//  or NULL if it doesn't exist.
Instr *get_instr(char *mnem, char *arg1, char *arg2)
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

// get_unaoper: returns the unary operator definition with matching string
Oper *get_unaoper(char *s)
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

// get_binoper: returns the binary operator definition with matching string
Oper *get_binoper(char *s)
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