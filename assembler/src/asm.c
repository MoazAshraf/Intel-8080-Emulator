// asm.c: assembler main source file
//
// Copyright (c) 2020 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdlib.h>
#include "asm.h"

char *prog; // name of the command that starts the program

char *cat_files(int argc, char **argv);
int get_statements(char *, Statement *);
int assemble(Statement *, int, char *);

// assembles Intel 8080 source code.
int main(int argc, char *argv[])
{
    prog = argv[0];         // get the name of the command
    char *srcbuf;           // source code buffer

    // load argument files or read from stdin
    srcbuf = cat_files(argc-1, argv+1);
    
    // allocate memory and collect statements
    Statement *statements = (Statement *) calloc(MAX_STMNTS, sizeof(Statement));
    int nstmnt = get_statements(srcbuf, statements);
    
    // free input source buffer
    free(srcbuf);

    // start assembly
    char outbuf[MAX_PROG];
    int outsize = assemble(statements, nstmnt, outbuf);

    // TODO: output machine code
    // TODO: free all open buffers and pointers

    return 0;
}