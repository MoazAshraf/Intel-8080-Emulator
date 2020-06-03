// asm.c: assembler main source file

#include <stdlib.h>
#include "io_utils.h"
#include "asm.h"

int get_statements(char *, Statement *);
int assemble(Statement *, int, char *);

// assembles Intel 8080 source code.
int main(int argc, char *argv[])
{
    // prog = argv[0];         // get the name of the command
    char *srcbuf;           // source code buffer

    if (argc == 1) {
        srcbuf = read_until_eof(stdin);
    } else {
        // TODO: load and concatenate files from arguments
        // srcbuf = cat_files(argc-1, argv+1);
    }
    
    // TODO: collect statements
    // Statement *statements = (Statement *) calloc(MAX_STMNTS, sizeof(Statement));
    // int nstmnt = get_statements(srcbuf, statements);
    
    // TODO: free input source buffer
    // free(srcbuf);

    // TODO: start assembly
    // char outbuf[MAX_PROG];
    // int outsize = assemble(statements, nstmnt, outbuf);

    // TODO: output machine code
    // TODO: free all open buffers and pointers

    return 0;
}