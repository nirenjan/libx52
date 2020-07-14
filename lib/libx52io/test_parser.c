/*
 * Saitek X52 IO driver - Parser test suite
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "io_common.h"
#include "usb-ids.h"

static int group_setup(void **state)
{
    libx52io_context *ctx;
    int rc;

    rc = libx52io_init(&ctx);
    if (rc != LIBX52IO_SUCCESS) {
        return rc;
    }

    *state = ctx;
    return 0;
}

#define TEST_SETUP(prodid) test_setup_ ## prodid

#define TEST_SETUP_FUNCTION(prodid) static int TEST_SETUP(prodid)(void **state) { \
    libx52io_context *ctx = *state; \
    ctx->pid = X52_PROD_X52 ## prodid; \
    _x52io_set_report_parser(ctx); \
    return 0; \
}

TEST_SETUP_FUNCTION(_1);
TEST_SETUP_FUNCTION(_2);
TEST_SETUP_FUNCTION(PRO);

#undef TEST_SETUP_FUNCTION

static int test_teardown(void **state)
{
    libx52io_context *ctx = *state;

    libx52io_close(ctx);
    return 0;
}

static int group_teardown(void **state)
{
    libx52io_context *ctx = *state;

    libx52io_exit(ctx);
    return 0;
}

static void test_error_x52(void **state) {
    /* Verify that passing a buffer of the wrong size returns IO error */
    libx52io_context *ctx = *state;
    unsigned char data[15];
    int rc;

    rc = _x52io_parse_report(ctx, NULL, data, sizeof(data));
    assert_int_equal(rc, LIBX52IO_ERROR_IO);
}

static void test_error_pro(void **state) {
    /* Verify that passing a buffer of the wrong size returns IO error */
    libx52io_context *ctx = *state;
    unsigned char data[14];
    int rc;

    rc = _x52io_parse_report(ctx, NULL, data, sizeof(data));
    assert_int_equal(rc, LIBX52IO_ERROR_IO);
}

#include "test_parser_tests.c"

#define TEST_LIST
const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_error_x52, TEST_SETUP(_1), test_teardown),
    cmocka_unit_test_setup_teardown(test_error_x52, TEST_SETUP(_2), test_teardown),
    cmocka_unit_test_setup_teardown(test_error_pro, TEST_SETUP(PRO), test_teardown),
    #include "test_parser_tests.c"
};

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, group_setup, group_teardown);
    return 0;
}

