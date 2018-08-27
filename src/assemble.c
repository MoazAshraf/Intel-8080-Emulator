#include "asm.h"

// assemble: assemble an array of statements into Intel 8080 machine code,
//  return length of output.
int assemble(const Statement statements[], int nstmnt, char outbuf[])
{
    int pc;     // program counter value
    char *outp; // pointer in outbuf

    // TODO: collect all labels

    // iterate over all statements and assemble
    for (int i = 0; i < nstmnt; i++) {
        // store program counter value
        pc = statements[i].pc;

        // TODO: execute pseudo-instructions and macros
        // TODO: assemble instructions and data in outbuf
    }

    return outp-outbuf;
}