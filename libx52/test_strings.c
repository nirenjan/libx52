/*
 * Saitek X52 MFD & LED driver - String test suite
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "libx52.h"

#define TEST_STRINGIFY(name) do { \
    char expected[256]; \
    for (int i=-1; i < sizeof(name ## _map) / sizeof(name ## _map[0]); i++) { \
        if (name ## _map[i] != NULL) { \
            strncpy(expected, name ## _map[i], sizeof(expected)); \
        } else { \
            snprintf(expected, sizeof(expected), unknown_fmt, i); \
        } \
        assert_string_equal(expected, libx52_ ## name ## _to_str(i)); \
    } \
} while (0)

static void test_led_id_names(void **state)
{
    static const char * led_id_map[21] = {
        [LIBX52_LED_FIRE] = "Fire",
        [LIBX52_LED_A] = "A",
        [LIBX52_LED_B] = "B",
        [LIBX52_LED_D] = "D",
        [LIBX52_LED_E] = "E",
        [LIBX52_LED_T1] = "T1",
        [LIBX52_LED_T2] = "T2",
        [LIBX52_LED_T3] = "T3",
        [LIBX52_LED_POV] = "POV",
        [LIBX52_LED_CLUTCH] = "Clutch",
        [LIBX52_LED_THROTTLE] = "Throttle",
    };

    static const char *unknown_fmt = "Unknown LED ID %d";

    TEST_STRINGIFY(led_id);
}

static void test_led_state_names(void **state)
{
    static const char * led_state_map[6] = {
        [LIBX52_LED_STATE_OFF] = "off",
        [LIBX52_LED_STATE_ON] = "on",
        [LIBX52_LED_STATE_RED] = "red",
        [LIBX52_LED_STATE_AMBER] = "amber",
        [LIBX52_LED_STATE_GREEN] = "green",
    };

    static const char *unknown_fmt = "Unknown LED state %d";

    TEST_STRINGIFY(led_state);
}

static void test_clock_id_names(void **state) {
    static const char * clock_id_map[4] = {
        [LIBX52_CLOCK_1] = "primary",
        [LIBX52_CLOCK_2] = "secondary",
        [LIBX52_CLOCK_3] = "tertiary",
    };

    static const char *unknown_fmt = "Unknown clock ID %d";

    TEST_STRINGIFY(clock_id);
}

static void test_clock_format_names(void **state) {
    static const char * clock_format_map[3] = {
        [LIBX52_CLOCK_FORMAT_12HR] = "12 hour",
        [LIBX52_CLOCK_FORMAT_24HR] = "24 hour",
    };

    static const char *unknown_fmt = "Unknown clock format %d";

    TEST_STRINGIFY(clock_format);
}

static void test_date_format_names(void **state) {
    static const char * date_format_map[4] = {
        [LIBX52_DATE_FORMAT_DDMMYY] = "DD-MM-YY",
        [LIBX52_DATE_FORMAT_MMDDYY] = "MM-DD-YY",
        [LIBX52_DATE_FORMAT_YYMMDD] = "YY-MM-DD",
    };

    static const char *unknown_fmt = "Unknown date format %d";

    TEST_STRINGIFY(date_format);
}

#define libx52_error_to_str libx52_strerror

static void test_strerror(void **state) {
    static const char *error_map[18] = {
        [LIBX52_SUCCESS] = "Success",
        [LIBX52_ERROR_INIT_FAILURE] = "Initialization failure",
        [LIBX52_ERROR_OUT_OF_MEMORY] = "Insufficient memory",
        [LIBX52_ERROR_INVALID_PARAM] = "Invalid parameter",
        [LIBX52_ERROR_NOT_SUPPORTED] = "Operation not supported",
        [LIBX52_ERROR_TRY_AGAIN] = "Try again",
        [LIBX52_ERROR_OUT_OF_RANGE] = "Input parameter out of range",
        [LIBX52_ERROR_USB_FAILURE] = "USB transaction failure",
        [LIBX52_ERROR_IO] = "USB input/output error",
        [LIBX52_ERROR_PERM] = "Access denied",
        [LIBX52_ERROR_NO_DEVICE] = "No such device",
        [LIBX52_ERROR_NOT_FOUND] = "Entity not found",
        [LIBX52_ERROR_BUSY] = "Resource busy",
        [LIBX52_ERROR_TIMEOUT] = "Operation timeout",
        [LIBX52_ERROR_OVERFLOW] = "Overflow",
        [LIBX52_ERROR_PIPE] = "Pipe error",
        [LIBX52_ERROR_INTERRUPTED] = "System call interrupted",
    };

    static const char *unknown_fmt = "Unknown error %d";

    TEST_STRINGIFY(error);
}

static const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_led_id_names),
    cmocka_unit_test(test_led_state_names),
    cmocka_unit_test(test_clock_id_names),
    cmocka_unit_test(test_clock_format_names),
    cmocka_unit_test(test_date_format_names),
    cmocka_unit_test(test_strerror),
};

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
