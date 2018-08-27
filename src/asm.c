// asm.c: Intel 8080 Assembler source file
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdlib.h>
#include "asm.h"

char *prog; // program name

char *cat_files(int argc, char **argv);
int get_statements(char *, Statement *);
int assemble(const Statement *, int, char *);

// assembles Intel 8080 source code.
int main(int argc, char *argv[])
{
    prog = argv[0];         // get the command line name
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