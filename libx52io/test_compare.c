/*
 * Saitek X52 IO driver - Compare test suite
 *
 * Copyright (C) 2012-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "common.h"

#define TEST_AXIS \
    FUNC(AXIS_X) \
    FUNC(AXIS_Y) \
    FUNC(AXIS_RZ) \
    FUNC(AXIS_Z) \
    FUNC(AXIS_RX) \
    FUNC(AXIS_RY) \
    FUNC(AXIS_SLIDER) \
    FUNC(AXIS_THUMBX) \
    FUNC(AXIS_THUMBY) \
    FUNC(AXIS_HATX) \
    FUNC(AXIS_HATY)

#define FUNC(axis_name) static void test_ ## axis_name (void **state) \
    { \
        (void)state; \
        libx52io_report prev = { 0 }; \
        libx52io_report curr = { 0 }; \
        uint64_t diff; \
        curr.axis[LIBX52IO_ ## axis_name] = 1; \
        diff = libx52io_compare_reports(&prev, &curr); \
        assert_int_equal(diff, LIBX52IO_REPORT_ ## axis_name); \
    }

TEST_AXIS
#undef FUNC

#define TEST_BUTTON \
    FUNC(BTN_TRIGGER) \
    FUNC(BTN_TRIGGER_2) \
    FUNC(BTN_FIRE) \
    FUNC(BTN_PINKY) \
    FUNC(BTN_A) \
    FUNC(BTN_B) \
    FUNC(BTN_C) \
    FUNC(BTN_D) \
    FUNC(BTN_E) \
    FUNC(BTN_T1_UP) \
    FUNC(BTN_T1_DN) \
    FUNC(BTN_T2_UP) \
    FUNC(BTN_T2_DN) \
    FUNC(BTN_T3_UP) \
    FUNC(BTN_T3_DN) \
    FUNC(BTN_POV_1_N) \
    FUNC(BTN_POV_1_E) \
    FUNC(BTN_POV_1_S) \
    FUNC(BTN_POV_1_W) \
    FUNC(BTN_POV_2_N) \
    FUNC(BTN_POV_2_E) \
    FUNC(BTN_POV_2_S) \
    FUNC(BTN_POV_2_W) \
    FUNC(BTN_CLUTCH) \
    FUNC(BTN_MOUSE_PRIMARY) \
    FUNC(BTN_MOUSE_SECONDARY) \
    FUNC(BTN_MOUSE_SCROLL_UP) \
    FUNC(BTN_MOUSE_SCROLL_DN) \
    FUNC(BTN_FUNCTION) \
    FUNC(BTN_START_STOP) \
    FUNC(BTN_RESET) \
    FUNC(BTN_PG_UP) \
    FUNC(BTN_PG_DN) \
    FUNC(BTN_UP) \
    FUNC(BTN_DN) \
    FUNC(BTN_SELECT) \
    FUNC(BTN_MODE_1) \
    FUNC(BTN_MODE_2) \
    FUNC(BTN_MODE_3)

#define FUNC(button_name) static void test_ ## button_name (void **state) \
    { \
        (void)state; \
        libx52io_report prev = { 0 }; \
        libx52io_report curr = { 0 }; \
        uint64_t diff; \
        curr.button[LIBX52IO_ ## button_name] = 1; \
        diff = libx52io_compare_reports(&prev, &curr); \
        assert_int_equal(diff, LIBX52IO_REPORT_ ## button_name); \
    }

TEST_BUTTON
#undef FUNC

static void test_mode(void **state)
{
    (void)state;
    libx52io_report prev = { 0 };
    libx52io_report curr = { 0 };
    uint64_t diff;
    curr.mode = 1;
    diff = libx52io_compare_reports(&prev, &curr);
    assert_int_equal(diff, LIBX52IO_REPORT_MODE);
}

static void test_hat(void **state)
{
    (void)state;
    libx52io_report prev = { 0 };
    libx52io_report curr = { 0 };
    uint64_t diff;
    curr.hat = 1;
    diff = libx52io_compare_reports(&prev, &curr);
    assert_int_equal(diff, LIBX52IO_REPORT_HAT);
}

#define FUNC(x) cmocka_unit_test(test_ ## x),

const struct CMUnitTest tests[] = {
    TEST_AXIS
    TEST_BUTTON
    FUNC(mode)
    FUNC(hat)
};
#undef FUNC

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
