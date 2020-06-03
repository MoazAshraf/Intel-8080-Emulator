// Unit test runner entry point

#include <check.h>
#include <stdlib.h>

// test suite declarations
Suite *get_token_test_suite(void);
Suite *read_until_of_test_suite(void);

int main(void)
{
    int no_failed = 0;
    SRunner *runner;

    // create suite runner and add all test suites 
    runner = srunner_create(get_token_test_suite());
    srunner_add_suite(runner, read_until_of_test_suite());

    // run all test suites
    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}