// Unit tests for read_until_eof() in io_utils.c

#include <check.h>
#include <stdio.h> 
#include <errno.h>
#include <string.h>
#include "../src/io_utils.h"

extern int errno;

char textfile_content[] = "This is a sample text file.\n"
                          "This is the second line.\n"
                          "...\n"
                          "This is the last line.";

START_TEST(test_read_until_eof)
{
    FILE *file = fopen("tests/examples/textfile.txt", "r");
    char *content;
    
    if (file == NULL)
        ck_abort_msg("Error opening file: %s", strerror(errno));
    else {
        content = read_until_eof(file);
        fclose(file);
        ck_assert_str_eq(content, textfile_content);
    }
}
END_TEST

Suite *read_until_of_test_suite(void)
{
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("read_until_eof");
    tc_core = tcase_create("core");

    // add unit tests
    tcase_add_test(tc_core, test_read_until_eof);

    suite_add_tcase(suite, tc_core);

    return suite;
}