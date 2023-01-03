/*
 * Saitek X52 IO driver - Axis test suite
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

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

static int test_setup(void **state)
{
    libx52io_context *ctx = *state;

    /* Create a dummy handle so that the test cases don't abort early */
    ctx->handle = (void *)(uintptr_t)(-1);

    return 0;
}

static int test_teardown(void **state)
{
    libx52io_context *ctx = *state;

    ctx->handle = NULL;
    libx52io_close(ctx);
    return 0;
}

static int group_teardown(void **state)
{
    libx52io_context *ctx = *state;

    ctx->handle = NULL;
    libx52io_exit(ctx);
    return 0;
}

/* List of test cases - list the axis and the corresponding maximum for that axis */
#define TEST_CASES \
    TEST_X52(X, 0, 2047) \
    TEST_X52(Y, 0, 2047) \
    TEST_X52(RZ, 0, 1023) \
    TEST_X52(Z, 0, 255) \
    TEST_X52(RX, 0, 255) \
    TEST_X52(RY, 0, 255) \
    TEST_X52(SLIDER, 0, 255) \
    TEST_X52(THUMBX, 0, 15) \
    TEST_X52(THUMBY, 0, 15) \
    TEST_X52(HATX, -1, 1) \
    TEST_X52(HATY, -1, 1) \
    TEST_PRO(X, 0, 1023) \
    TEST_PRO(Y, 0, 1023) \
    TEST_PRO(RZ, 0, 1023) \
    TEST_PRO(Z, 0, 255) \
    TEST_PRO(RX, 0, 255) \
    TEST_PRO(RY, 0, 255) \
    TEST_PRO(SLIDER, 0, 255) \
    TEST_PRO(THUMBX, 0, 15) \
    TEST_PRO(THUMBY, 0, 15) \
    TEST_PRO(HATX, -1, 1) \
    TEST_PRO(HATY, -1, 1)

#define TEST_X52(axis, min, max) TEST(_1, axis, min, max) TEST(_2, axis, min, max)
#define TEST_PRO(axis, min, max) TEST(PRO, axis, min, max)

#define TEST(prodid, axis, minval, maxval) \
    static void axis ## _ ## prodid (void **state) \
    { \
        libx52io_context *ctx = *state; \
        int rc; \
        int32_t min = 0; \
        int32_t max = 0; \
        ctx->pid = X52_PROD_X52 ## prodid; \
        _x52io_set_axis_range(ctx); \
        rc = libx52io_get_axis_range(ctx, LIBX52IO_AXIS_ ## axis, &min, &max); \
        assert_int_equal(rc, LIBX52IO_SUCCESS); \
        assert_int_equal(min, minval); \
        assert_int_equal(max, maxval); \
    }

TEST_CASES
#undef TEST

static void test_error_case(void **state)
{
    libx52io_context *ctx = *state;
    int rc;
    int32_t min, max;

    rc = libx52io_get_axis_range(NULL, 0, NULL, NULL);
    assert_int_equal(rc, LIBX52IO_ERROR_INVALID);

    rc = libx52io_get_axis_range(ctx, 0, NULL, NULL);
    assert_int_equal(rc, LIBX52IO_ERROR_INVALID);

    rc = libx52io_get_axis_range(ctx, 0, &min, NULL);
    assert_int_equal(rc, LIBX52IO_ERROR_INVALID);

    rc = libx52io_get_axis_range(ctx, -1, &min, &max);
    assert_int_equal(rc, LIBX52IO_ERROR_INVALID);

    rc = libx52io_get_axis_range(ctx, LIBX52IO_AXIS_MAX, &min, &max);
    assert_int_equal(rc, LIBX52IO_ERROR_INVALID);

    ctx->handle = NULL;
    rc = libx52io_get_axis_range(ctx, LIBX52IO_AXIS_X, &min, &max);
    assert_int_equal(rc, LIBX52IO_ERROR_NO_DEVICE);
}

#define TEST(prodid, axis, minval, maxval) cmocka_unit_test_setup_teardown(axis ## _ ## prodid, test_setup, test_teardown),

const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_error_case, test_setup, test_teardown),
    TEST_CASES
};
#undef TEST

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, group_setup, group_teardown);
    return 0;
}

