// Header file for io_utils.c

#define MAX_LINE_SIZE   1024    // buffer size for reading lines
#define INIT_BUF_SIZE   1024    // initial content buffer size

char *read_until_eof(FILE *stream);