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

START_TEST(test_get_token_dec)
{
    Token tok;
    
    get_token("328D", &tok);
    ck_assert_str_eq(tok.str, "328");
    ck_assert_int_eq(tok.type, TOK_DEC);
    
    get_token("612", &tok);
    ck_assert_str_eq(tok.str, "612");
    ck_assert_int_eq(tok.type, TOK_DEC);

    get_token("1024124d", &tok);
    ck_assert_str_eq(tok.str, "1024124");
    ck_assert_int_eq(tok.type, TOK_DEC);
}
END_TEST

START_TEST(test_get_token_hex)
{
    Token tok;
    
    get_token("0FFFH", &tok);
    ck_assert_str_eq(tok.str, "0FFF");
    ck_assert_int_eq(tok.type, TOK_HEX);
    
    get_token("0bah", &tok);
    ck_assert_str_eq(tok.str, "0ba");
    ck_assert_int_eq(tok.type, TOK_HEX);

    get_token("10fa23dbh", &tok);
    ck_assert_str_eq(tok.str, "10fa23db");
    ck_assert_int_eq(tok.type, TOK_HEX);
}
END_TEST

START_TEST(test_get_token_bin)
{
    Token tok;
    
    get_token("0101B", &tok);
    ck_assert_str_eq(tok.str, "0101");
    ck_assert_int_eq(tok.type, TOK_BIN);
    
    get_token("10010b", &tok);
    ck_assert_str_eq(tok.str, "10010");
    ck_assert_int_eq(tok.type, TOK_BIN);

    get_token("100100101b", &tok);
    ck_assert_str_eq(tok.str, "100100101");
    ck_assert_int_eq(tok.type, TOK_BIN);
}
END_TEST

START_TEST(test_get_token_ascii)
{
    Token tok;
    
    get_token("'O'", &tok);
    ck_assert_str_eq(tok.str, "O");
    ck_assert_int_eq(tok.type, TOK_ASCII);
    
    get_token("'?'", &tok);
    ck_assert_str_eq(tok.str, "?");
    ck_assert_int_eq(tok.type, TOK_ASCII);

    get_token("'ab'", &tok);
    ck_assert_str_eq(tok.str, "ab");
    ck_assert_int_eq(tok.type, TOK_ASCII);
}
END_TEST

Suite *tokenize_test_suite(void)
{
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("tokenize");
    tc_core = tcase_create("core");

    // add unit tests
    tcase_add_test(tc_core, test_get_token_dec);
    tcase_add_test(tc_core, test_get_token_hex);
    tcase_add_test(tc_core, test_get_token_bin);
    tcase_add_test(tc_core, test_get_token_ascii);

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