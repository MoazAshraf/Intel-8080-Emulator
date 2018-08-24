// tokenize.c: Assembler lexical analysis, syntax checking
//  and program statement collection
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "asm.h"

int get_arguments(char *, const Instr *, Args *);
int get_argument(char *, Token **, int *);
int get_token(char *, Token *);
int getword(char *, char *, int);
Instr *get_instr(char *, char *, char *);
Oper *get_unaoper(char *);
Oper *get_binoper(char *);
char *validate_label(char *, const Statement *, int);
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
                    exit(EXIT_FAILURE);
                }

                // store instruction
                if (statementp->instr == NULL)
                    statementp->instr = mnem;
                else
                    (++statementp)->instr = mnem;   // new statement

                // collect arguments
                srcp += get_arguments(srcp, insinfo, &statementp->args);

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
            exit(EXIT_FAILURE);
        }
    }

    return statementp - statements +
        ((statementp->label || statementp->instr) ? 1 : 0);
}

// get_arguments: collects instruction arguments, returns #characters read.
int get_arguments(char buf[], const Instr *insinfo, Args *args)
{
    char *bufp = buf;   // pointer in buf

    // collect arguments
    if (insinfo->nargs > 0) {
        int nread;  // #characters read from buffer

        // collect first argument
        bufp += nread = get_argument(bufp, &args->arg1, &args->ntok1);
        if (nread == 0) {
            printerr("error: instruction %s takes %d arguments. "
                "None provided.", insinfo->mnem, insinfo->nargs);
            exit(EXIT_FAILURE);
        } else {
            if (insinfo->nargs == 2) {

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
                
                // collect comma
                if (*bufp != ',') {
                    if (iswdchr(*bufp))
                        printerr("error: instruction %s takes %d "
                            "arguments. 1 provided.",
                            insinfo->mnem, insinfo->nargs);
                    else
                        printerr("error: unexpected token '%c'", *bufp);
                    exit(EXIT_FAILURE);
                } else
                    bufp++;

                // collect second argument
                bufp += nread = get_argument(bufp, &args->arg2, &args->ntok2);
                if (nread == 0) {
                    printerr("error: instruction %s takes %d arguments. "
                        "1 provided.", insinfo->mnem, insinfo->nargs);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    args->nargs = insinfo->nargs;
    
    return bufp-buf;
}

// get_argument: collects an argument expression, returns #characters read.
int get_argument(char buf[], Token **arg, int *ntoks)
{
    char *bufp;             // pointer in buf
    int nread;              // number of characters read from buffer
    Token toks[MAX_TOKENS]; // temporary token storage
    Token *tokp;            // pointer in toks
    Token *argp;            // pointer in *arg
    int nparen;             // parentheses balance

    // collect tokens in toks
    bufp = buf;
    tokp = toks;
    nparen = 0; 
    for (*ntoks = 0; *ntoks < MAX_TOKENS && (nread = get_token(bufp, tokp)) > 0
     && tokp->type != -1; tokp++, (*ntoks)++) { 
        // increment buffer
        bufp += nread;

        // unary and binary + and - type check
        if (*tokp->str == '+' || *tokp->str == '-') {
            if (*ntoks > 0 && (isoperand((tokp-1)->type) || *(tokp-1)->str == ')'))
                tokp->type = TOK_BINOPER;
            else
                tokp->type = TOK_UNAOPER;
        }

        // syntax check
        if (tokp->type == TOK_BINOPER && (*ntoks == 0
         || !isoperand((tokp-1)->type) && *(tokp-1)->str != ')')) {

            printerr("error: unexpected operand %s", tokp->str);
            exit(EXIT_FAILURE);

        } else if (tokp->type == TOK_LABEL && *ntoks > 0
         && (isoperand((tokp-1)->type) || *(tokp-1)->str == ')')) {
            bufp -= nread;
            break;
        } else if (isoperand(tokp->type) && *ntoks > 0
         && (isoperand((tokp-1)->type) || (tokp-1)->type == TOK_INSTR
         || *(tokp-1)->str == ')')) {

            printerr("error: unexpected token %s", tokp->str);
            exit(EXIT_FAILURE);

        } else if (tokp->type == TOK_PAREN) {

            if (*tokp->str == '(')
                nparen++;
            else if (*tokp->str == ')') {
                nparen--; 
                if (nparen < 0) {
                    bufp -= nread;
                    break;
                }
            }
        } else if (tokp->type == TOK_INSTR) {
            if (*ntoks == 0 || *(tokp-1)->str != '(') {
                bufp -= nread;
                break;
            } else {  // collect nested instruction arguments
                Instr *insinfo;
                Args args;
                insinfo = get_instr(tokp->str, NULL, NULL);
                bufp += get_arguments(bufp, insinfo, &args);
                if (args.nargs > 0) {
                    // for the instruction token
                    tokp++;
                    (*ntoks)++;

                    // store first nested argument
                    Token *atokp = args.arg1;
                    while (*ntoks < MAX_TOKENS && atokp-args.arg1 < args.ntok1) {
                        tokp->type = atokp->type;
                        tokp->str = atokp->str;
                        atokp++;
                        tokp++;
                        (*ntoks)++;
                    }
                    if (args.nargs == 2) {
                        // store comma token
                        if (*ntoks < MAX_TOKENS) {
                            tokp->type = TOK_COMMA;
                            tokp->str = (char *) malloc(2);
                            strcpy(tokp->str, ",");
                            tokp++;
                            (*ntoks)++;
                        }

                        // store second nested argument
                        atokp = args.arg2;
                        while (*ntoks < MAX_TOKENS && atokp-args.arg2 < args.ntok2) {
                            tokp->type = atokp->type;
                            tokp->str = atokp->str;
                            atokp++;
                            tokp++;
                            (*ntoks)++;
                        }
                    }

                    // to balance instruction token
                    tokp--;
                    (*ntoks)--;
                }
            }
        }
    }
    // parentheses balance error
    if (nparen > 0) {
        printerr("error: missing closing )");
        exit(EXIT_FAILURE);
    }
    // else if (nparen < 0) {
    //     printerr("error: missing opening (");
    //     exit(EXIT_FAILURE);
    // }

    // if last token is an operator
    if ((toks+*ntoks-1)->type == TOK_BINOPER
     || (toks+*ntoks-1)->type == TOK_UNAOPER) {
        printerr("error: expected operand after %s", (toks+*ntoks-1)->str);
        exit(EXIT_FAILURE);
    }

    // store argument tokens
    *arg = (Token *) malloc(*ntoks * sizeof(Token));
    for (tokp = toks, argp = *arg; tokp-toks < *ntoks; tokp++, argp++) {
        argp->str = tokp->str;
        argp->type = tokp->type;
    }

    // return #characters read
    return bufp-buf;
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
            else
                word[--wlen] = '\0';

            switch (base) {
                case 'b':   // binary
                    while (wordp-word < wlen) {
                        if (*wordp-'0' < 2)
                            wordp++;
                        else {
                            printerr("error: binary number %s cannot contain %c",
                                word, *wordp);
                            exit(EXIT_FAILURE);
                        }
                    }
                    tok->type = TOK_BIN;
                    break;
                case 'o': case 'q': // octal
                    while (wordp-word < wlen) {
                        if (*wordp-'0' < 8)
                            wordp++;
                        else {
                            printerr("error: octal number %s cannot contain %c",
                                word, *wordp);
                            exit(EXIT_FAILURE);
                        }
                    }
                    tok->type = TOK_OCT;
                    break;
                case 'd':   // decimal
                    while (wordp-word < wlen) {
                        if (isdigit(*wordp))
                            wordp++;
                        else {
                            printerr("error: decimal number %s cannot contain %c",
                                word, *wordp);
                            exit(EXIT_FAILURE);
                        }
                    }
                    tok->type = TOK_DEC;
                    break;
                case 'h':   // hexadecimal
                    while (wordp-word < wlen) {
                        if (isxdigit(*wordp))
                            wordp++;
                        else {
                            printerr("error: hexadecimal number %s cannot "
                                "contain %c", word, *wordp);
                            exit(EXIT_FAILURE);
                        }
                    }
                    tok->type = TOK_HEX;
                    break;
                default:
                    printerr("error: illegal token %s", word);
                    exit(EXIT_FAILURE);
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
            *wordp = '\0';
            if (*bufp == '\'')
                bufp++;
            else {
                printerr("error: missing ASCII delimiter");
                exit(EXIT_FAILURE);
            }
            tok->type = TOK_ASCII;
        } else if (*word) {
            bufp--;
            *word = '\0';
            tok->type = -1;
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
        exit(EXIT_FAILURE);
    }

    // TODO: check if label is pseudo-instruction
    // check if label is instruction
    if (get_instr(label, NULL, NULL) != NULL) {
        printerr("error: %s is a defined mnemonic; cannot be used as a label", label);
        exit(EXIT_FAILURE);
    }

    // check for duplicate labels
    for (int i = 0; i < nstmnt; i++)
        if (statements[i].label && strcicmp(statements[i].label, label) == 0) {
            printerr("error: label '%s' is a duplicate", label);
            exit(EXIT_FAILURE);
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