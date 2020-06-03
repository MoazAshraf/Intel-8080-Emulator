// Contains functions for reading input, writing output and dealing with files

#include "io_utils.h"

#include <stdlib.h>
#include <string.h>

/**
 * Reads from an file stream until it encounters EOF
 * 
 * Note: this function uses dynamic allocation, make sure to free the output
 * buffer when done
 * 
 * Arguments:
 *   FILE *stream: the file stream to read from 
 * 
 * Returns a pointer to the output buffer on success and NULL on failure
 */
char *read_until_eof(FILE *stream)
{
    char line[MAX_LINE_SIZE];   // buffer to store lines
    size_t content_size = 1;    // includes null character

    // allocate some memory at the start (more efficient)
    char *content = malloc(sizeof(char) * INIT_BUF_SIZE);
    if (content == NULL) {
        perror("error: read_until_eof failed to allocate content");
        exit(EXIT_FAILURE);
    }
    content[0] = '\0';  // null character
    
    // reads lines from the stream until it encounters EOF
    while (fgets(line, MAX_LINE_SIZE, stream) != NULL) {
        char *old_content = content;
        content_size += strlen(line);
        content = realloc(content, content_size);
        if (content == NULL) {
            free(old_content);
            perror("error: read_until_eof failed to reallocate content");
            exit(EXIT_FAILURE);
        }
        strcat(content, line);
    }

    if (ferror(stream)) {
        free(content);
        perror("error: read_until_eof failed to read from stream");
        exit(EXIT_FAILURE);
    }

    return content;
}


// TODO: write a function that reads multiple files and concatenates them