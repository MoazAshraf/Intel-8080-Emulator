// Header file for io_utils.c

#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdio.h>

#define MAX_LINE_SIZE   1024    // buffer size for reading lines
#define INIT_BUF_SIZE   1024    // initial content buffer size

char *read_until_eof(FILE *stream);

#endif  // IO_UTILS_H