/*
 * Saitek X52 IO driver - io string parse tests
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libx52io.h"

static void lower_ascii_only(char *dst, const char *src)
{
    for (; *src; src++, dst++) {
        unsigned char c = (unsigned char)*src;

        if (c >= 'A' && c <= 'Z') {
            c = (unsigned char)(c - 'A' + 'a');
        }
        *dst = (char)c;
    }
    *dst = '\0';
}

static void test_axis_round_trip(void **state)
{
    libx52io_axis a;
    libx52io_axis out;

    (void)state;

    for (a = LIBX52IO_AXIS_X; a < LIBX52IO_AXIS_MAX; a++) {
        const char *s = libx52io_axis_to_str(a);

        assert_non_null(s);
        assert_int_equal(LIBX52IO_SUCCESS, libx52io_axis_from_str(s, &out));
        assert_int_equal(a, out);
        assert_ptr_equal(s, libx52io_axis_to_str(out));
    }
}

static void test_button_round_trip(void **state)
{
    libx52io_button b;
    libx52io_button out;

    (void)state;

    for (b = LIBX52IO_BTN_TRIGGER; b < LIBX52IO_BUTTON_MAX; b++) {
        const char *s = libx52io_button_to_str(b);

        assert_non_null(s);
        assert_int_equal(LIBX52IO_SUCCESS, libx52io_button_from_str(s, &out));
        assert_int_equal(b, out);
        assert_ptr_equal(s, libx52io_button_to_str(out));
    }
}

static void test_axis_from_str_invalid(void **state)
{
    libx52io_axis out;

    (void)state;

    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str("", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str("ABS_", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str("ABS_XX", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str("abs_x", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str("BTN_FIRE", &out));
}

static void test_button_from_str_invalid(void **state)
{
    libx52io_button out;

    (void)state;

    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str("", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str("BTN_", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID,
        libx52io_button_from_str("BTN_FIRE_EXTRA", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str("btn_fire", &out));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str("ABS_X", &out));
}

static void test_from_str_null_args(void **state)
{
    libx52io_axis ax;
    libx52io_button btn;

    (void)state;

    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str(NULL, &ax));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str("ABS_X", NULL));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str(NULL, &btn));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str("BTN_A", NULL));
}

static void test_axis_from_str_nocase(void **state)
{
    libx52io_axis a;
    libx52io_axis out;
    char buf[64];

    (void)state;

    for (a = LIBX52IO_AXIS_X; a < LIBX52IO_AXIS_MAX; a++) {
        const char *s = libx52io_axis_to_str(a);

        assert_non_null(s);
        lower_ascii_only(buf, s);
        assert_int_equal(LIBX52IO_SUCCESS, libx52io_axis_from_str_nocase(buf, &out));
        assert_int_equal(a, out);
    }

    assert_int_equal(LIBX52IO_SUCCESS,
        libx52io_axis_from_str_nocase("aBs_hAtX", &out));
    assert_int_equal(LIBX52IO_AXIS_HATX, out);
}

static void test_button_from_str_nocase(void **state)
{
    libx52io_button b;
    libx52io_button out;
    char buf[64];

    (void)state;

    for (b = LIBX52IO_BTN_TRIGGER; b < LIBX52IO_BUTTON_MAX; b++) {
        const char *s = libx52io_button_to_str(b);

        assert_non_null(s);
        lower_ascii_only(buf, s);
        assert_int_equal(LIBX52IO_SUCCESS, libx52io_button_from_str_nocase(buf, &out));
        assert_int_equal(b, out);
    }

    assert_int_equal(LIBX52IO_SUCCESS,
        libx52io_button_from_str_nocase("BtN_t1_dN", &out));
    assert_int_equal(LIBX52IO_BTN_T1_DN, out);
}

static void test_from_str_nocase_invalid(void **state)
{
    libx52io_axis ax;
    libx52io_button btn;

    (void)state;

    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str_nocase("", &ax));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str_nocase("abs_xx", &ax));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str_nocase("", &btn));
    assert_int_equal(LIBX52IO_ERROR_INVALID,
        libx52io_button_from_str_nocase("btn_fire_extra", &btn));
}

static void test_from_str_nocase_null_args(void **state)
{
    libx52io_axis ax;
    libx52io_button btn;

    (void)state;

    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str_nocase(NULL, &ax));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_axis_from_str_nocase("abs_x", NULL));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str_nocase(NULL, &btn));
    assert_int_equal(LIBX52IO_ERROR_INVALID, libx52io_button_from_str_nocase("btn_a", NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_axis_round_trip),
        cmocka_unit_test(test_button_round_trip),
        cmocka_unit_test(test_axis_from_str_invalid),
        cmocka_unit_test(test_button_from_str_invalid),
        cmocka_unit_test(test_from_str_null_args),
        cmocka_unit_test(test_axis_from_str_nocase),
        cmocka_unit_test(test_button_from_str_nocase),
        cmocka_unit_test(test_from_str_nocase_invalid),
        cmocka_unit_test(test_from_str_nocase_null_args),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
