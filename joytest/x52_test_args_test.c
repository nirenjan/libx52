/*
 * Saitek X52 Pro MFD & LED driver - x52test argv parsing unit tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "x52_test_args.h"

#define BIT_A (1 << 0)
#define BIT_B (1 << 1)
#define BIT_C (1 << 2)
#define TEST_ALL_SYN (BIT_A | BIT_B | BIT_C)

static const struct x52test_cmd syn_cmds[] = {
    { "a", BIT_A },
    { "b", BIT_B },
    { "c", BIT_C },
    { NULL, 0 },
};

static void test_argc_one_is_test_all(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(1, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_OK);
    assert_int_equal(r.test_bitmap, TEST_ALL_SYN);
}

static void test_single_cmd(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"b", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(2, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_OK);
    assert_int_equal(r.test_bitmap, BIT_B);
}

static void test_multi_cmd_ors(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"a", (char *)"c", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(3, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_OK);
    assert_int_equal(r.test_bitmap, BIT_A | BIT_C);
}

static void test_duplicate_cmd_ors_same_bit(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"a", (char *)"a", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(3, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_OK);
    assert_int_equal(r.test_bitmap, BIT_A);
}

static void test_help_keyword(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"help", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(2, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_HELP);
}

static void test_dashdash_help(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"--help", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(2, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_HELP);
}

static void test_help_after_other_arg(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"a", (char *)"help", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(3, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_HELP);
}

static void test_help_before_other_arg(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"help", (char *)"a", (char *)"b", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(4, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_HELP);
    assert_int_equal(r.test_bitmap, 0);
}

static void test_unknown_bad_index(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"a", (char *)"nope", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(3, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_UNKNOWN);
    assert_int_equal(r.bad_arg_index, 2);
}

static void test_unknown_first_arg_index(void **state)
{
    (void)state;
    char *argv[] = { (char *)"prog", (char *)"xyz", NULL };
    struct x52test_argv_result r;

    x52test_parse_argv(2, argv, syn_cmds, TEST_ALL_SYN, &r);
    assert_int_equal(r.status, X52TEST_ARGV_UNKNOWN);
    assert_int_equal(r.bad_arg_index, 1);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_argc_one_is_test_all),
        cmocka_unit_test(test_single_cmd),
        cmocka_unit_test(test_multi_cmd_ors),
        cmocka_unit_test(test_duplicate_cmd_ors_same_bit),
        cmocka_unit_test(test_help_keyword),
        cmocka_unit_test(test_dashdash_help),
        cmocka_unit_test(test_help_after_other_arg),
        cmocka_unit_test(test_help_before_other_arg),
        cmocka_unit_test(test_unknown_bad_index),
        cmocka_unit_test(test_unknown_first_arg_index),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
