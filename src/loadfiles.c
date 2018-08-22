// loadfiles.c: loading, concatenating and buffering input files
//
// Copyright (c) 2018 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXREAD 8192    // buffer size for reading from stdin

void printerr(char *, ...);

// cat_files: reads from command line files or stdin into a buffer
char *cat_files(int filec, char *filev[])
{
    char *catbuf;           // buffer to hold concatenated files
    char *catp;             // pointer in catbuf
    long unsigned bufsize;  // catbuf buffer size
    int filecount = filec;

    if (filec == 0) {
        char readbuf[MAXREAD];  // buffer for reading
        char *rbufp;            // pointer in readbuf
        char c;

        // read into readbuf
        for (rbufp = readbuf; rbufp-readbuf < MAXREAD && (c = getchar()) != EOF;)
            *rbufp++ = c;

        // copy to catbuf
        bufsize = rbufp-readbuf;
        if ((catbuf = (char *) malloc(bufsize+1)) == NULL) {    // +1 for terminating '\0'
            printerr("error: cannot allocate memory");
            exit(EXIT_FAILURE);
        }
        strncpy(catbuf, readbuf, bufsize);
    } else {
        FILE *fp;           // file pointer
        char **fbufs;       // file buffers

        if ((fbufs = (char **) malloc(filec * sizeof(char**))) == NULL) {
            printerr("error: cannot allocate memory");
            exit(EXIT_FAILURE);
        }
        char **fbp = fbufs; // pointer in fbufs
        bufsize = 0;

        for ( ; filec > 0; filec--, filev++) {
            // open file stream
            if ((fp = fopen(*filev, "rb")) == NULL) {
                printerr("error: cannot open file %s", *filev);
                exit(EXIT_FAILURE);
            }
            
            // get size of file
            fseek(fp, 0L, SEEK_END);
            long unsigned fsize = ftell(fp);
            bufsize += fsize;
            fseek(fp, 0L, SEEK_SET);

            // read file in a buffer
            if ((*fbp = (char *) malloc(fsize+1)) == NULL) {
                printerr("error: cannot allocate memory");
                exit(EXIT_FAILURE);
            }
            if (fread(*fbp, 1, fsize, fp) != fsize) {
                printerr("error: cannot read file %s", *filev);
                exit(EXIT_FAILURE);
            }
            (*fbp)[fsize] = '\0';
            fbp++;
            fclose(fp); // close file stream
        }

        // concatenate all files into catbuf
        if ((catbuf = (char *) malloc(bufsize+1)) == NULL) {
            printerr("error: cannot allocate memory");
            exit(EXIT_FAILURE);
        }
        catp = catbuf;
        fbp = fbufs;

        for (filec = filecount; filec > 0; filec--) {
            strcpy(catp, *fbp);
            catp += strlen(*fbp);
            free(*fbp);
            fbp++;
        }

        free(fbufs);
    }
    catbuf[bufsize] = '\0';
    return catbuf;
}