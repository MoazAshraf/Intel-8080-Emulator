// tests for get_token() in tokenize.c
//
// Copyright (c) 2020 Moaz Ashraf
// Licensed under MIT License
// Refer to LICENSE file

#include <stdlib.h>
#include <check.h>
#include "../src/asm.h"

char *prog;
int get_token(char buf[], Token *tok);

START_TEST(test_get_token_decimal)
{
    Token tok;
    
    get_token("328D", &tok);
    ck_assert_str_eq(tok.str, "328");
    ck_assert_int_eq(tok.type, TOK_DEC);
}
END_TEST

Suite *tokenize_test_suite(void)
{
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("tokenize");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_get_token_decimal);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main(void)
{
    prog = "check_tests";

    int no_failed = 0;
    Suite *suite;
    SRunner *runner;

    suite = tokenize_test_suite();
    runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}