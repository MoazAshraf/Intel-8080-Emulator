#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXREAD 8192    // buffer size for reading from stdin

char *prog; // program name

void printerr(char *, ...);

// assembles Intel 8080 source code.
int main(int argc, char *argv[])
{

    prog = argv[0];             // get the command line name
    char *srcbuf;               // source code buffer
    char *srcp;                 // pointer in srcbuf
    long unsigned srcsize;      // source code buffer size

    if (argc == 1) {
        char readbuf[MAXREAD];  // buffer for reading
        char *rbufp;            // pointer in readbuf
        char c;

        // read into readbuf
        for (rbufp = readbuf; rbufp-readbuf < MAXREAD
         && (c = getchar()) != EOF;)
            *rbufp++ = c;

        // copy to srcbuf
        srcsize = rbufp-readbuf;
        if ((srcbuf = (char *) malloc(srcsize+1)) == NULL) {    // +1 for terminating '\0'
            printerr("error: cannot allocate memory");
            return 1;
        }
        strncpy(srcbuf, readbuf, srcsize);
    } else {
        int fcnt = argc-1;  // file count
        FILE *fp;           // file pointer
        char **fbufs;       // file buffers

        if ((fbufs = (char **) malloc(fcnt * sizeof(char**))) == NULL) {
            printerr("error: cannot allocate memory");
            return 1;
        }
        char **fbp = fbufs; // pointer in fbufs
        srcsize = 0;

        while (--argc > 0) {
            // open file stream
            if ((fp = fopen(*++argv, "rb")) == NULL) {
                printerr("error: cannot open file %s", *argv);
                return 1;
            }
            
            // get size of file
            fseek(fp, 0L, SEEK_END);
            long unsigned fsize = ftell(fp);
            srcsize += fsize;
            fseek(fp, 0L, SEEK_SET);

            // read file in a buffer
            if ((*fbp = (char *) malloc(fsize+1)) == NULL) {
                printerr("error: cannot allocate memory");
                return 1;
            }
            if (fread(*fbp, 1, fsize, fp) != fsize) {
                printerr("error: cannot read file %s", *argv);
                return 1;
            }
            (*fbp)[fsize] = '\0';
            fbp++;
            fclose(fp); // close file stream
        }

        // concatenate all files into srcbuf
        if ((srcbuf = (char *) malloc(srcsize+1)) == NULL) {
            printerr("error: cannot allocate memory");
            return 1;
        }
        srcp = srcbuf;
        fbp = fbufs;
        argc = fcnt+1;

        while (--argc > 0) {
            strcpy(srcp, *fbp);
            srcp += strlen(*fbp);
            free(*fbp);
            fbp++;
        }

        free(fbufs);
    }
    srcbuf[srcsize] = '\0';
    // TODO: collect labels, instructions and pseudo instructions
    free(srcbuf);
    // TODO: start assembly
    // TODO: output machine code
    // TODO: free any other buffers
    return 0;
}

#include <stdarg.h>

// printerr: prints a formatted error message.
void printerr(char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    fprintf(stderr, "%s: ", prog);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}