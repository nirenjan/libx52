/*
 * Saitek X52 Pro MFD & LED driver - Event test dispatch unit tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "ev_denoise.h"
#include "ev_dispatch.h"

static void test_format_events_no_change(void **state)
{
    (void)state;
    char buffer[1024] = {0};
    FILE *stream = fmemopen(buffer, sizeof(buffer), "w");
    assert_non_null(stream);

    struct timeval tv = { .tv_sec = 1234567890, .tv_usec = 123456 };
    libx52io_report last = {0};
    libx52io_report curr = {0};
    int32_t denoise_mask[LIBX52IO_AXIS_MAX] = {0};

    bool changed = evtest_format_events(stream, &tv, &last, &curr, denoise_mask, true);
    fclose(stream);

    assert_false(changed);
    assert_string_equal(buffer, "");
}

static void test_format_events_button_change(void **state)
{
    (void)state;
    char buffer[1024] = {0};
    FILE *stream = fmemopen(buffer, sizeof(buffer), "w");
    assert_non_null(stream);

    struct timeval tv = { .tv_sec = 1594431236, .tv_usec = 817842 };
    libx52io_report last = {0};
    libx52io_report curr = {0};
    int32_t denoise_mask[LIBX52IO_AXIS_MAX] = {0};

    curr.button[LIBX52IO_BTN_TRIGGER] = true;
    curr.button[LIBX52IO_BTN_FIRE] = true;

    bool changed = evtest_format_events(stream, &tv, &last, &curr, denoise_mask, false);
    fclose(stream);

    assert_true(changed);
    const char *expected =
        "Event @ 1594431236.817842: BTN_TRIGGER, value 1\n"
        "Event @ 1594431236.817842: BTN_FIRE, value 1\n"
        "\n";
    assert_string_equal(buffer, expected);
}

static void test_format_events_axis_change_denoise_disabled(void **state)
{
    (void)state;
    char buffer[1024] = {0};
    FILE *stream = fmemopen(buffer, sizeof(buffer), "w");
    assert_non_null(stream);

    struct timeval tv = { .tv_sec = 1594431236, .tv_usec = 817842 };
    libx52io_report last = {0};
    libx52io_report curr = {0};
    int32_t denoise_mask[LIBX52IO_AXIS_MAX] = {0};

    last.axis[LIBX52IO_AXIS_X] = 100;
    curr.axis[LIBX52IO_AXIS_X] = 101;

    bool changed = evtest_format_events(stream, &tv, &last, &curr, denoise_mask, false);
    fclose(stream);

    assert_true(changed);
    const char *expected =
        "Event @ 1594431236.817842: ABS_X, value 101\n"
        "\n";
    assert_string_equal(buffer, expected);
}

static void test_format_events_axis_change_denoise_filtered(void **state)
{
    (void)state;
    char buffer[1024] = {0};
    FILE *stream = fmemopen(buffer, sizeof(buffer), "w");
    assert_non_null(stream);

    struct timeval tv = { .tv_sec = 1594431236, .tv_usec = 817842 };
    libx52io_report last = {0};
    libx52io_report curr = {0};
    int32_t denoise_mask[LIBX52IO_AXIS_MAX] = {0};

    /* For max=1023, low 4 bits cleared. 100 & mask == 101 & mask */
    denoise_mask[LIBX52IO_AXIS_X] = evtest_axis_denoise_mask(1023);
    last.axis[LIBX52IO_AXIS_X] = 100;
    curr.axis[LIBX52IO_AXIS_X] = 101;

    bool changed = evtest_format_events(stream, &tv, &last, &curr, denoise_mask, true);
    fclose(stream);

    assert_false(changed);
    assert_string_equal(buffer, "");
}

static void test_format_events_axis_and_button_combined(void **state)
{
    (void)state;
    char buffer[1024] = {0};
    FILE *stream = fmemopen(buffer, sizeof(buffer), "w");
    assert_non_null(stream);

    struct timeval tv = { .tv_sec = 1594431236, .tv_usec = 817842 };
    libx52io_report last = {0};
    libx52io_report curr = {0};
    int32_t denoise_mask[LIBX52IO_AXIS_MAX] = {0};

    denoise_mask[LIBX52IO_AXIS_X] = evtest_axis_denoise_mask(1023);
    last.axis[LIBX52IO_AXIS_X] = 0;
    curr.axis[LIBX52IO_AXIS_X] = 512;

    last.button[LIBX52IO_BTN_MODE_1] = true;
    curr.button[LIBX52IO_BTN_MODE_1] = false;

    bool changed = evtest_format_events(stream, &tv, &last, &curr, denoise_mask, true);
    fclose(stream);

    assert_true(changed);
    const char *expected =
        "Event @ 1594431236.817842: ABS_X, value 512\n"
        "Event @ 1594431236.817842: BTN_MODE_1, value 0\n"
        "\n";
    assert_string_equal(buffer, expected);
}

static void test_init_denoise_masks_disabled(void **state)
{
    (void)state;
    int32_t denoise_mask[LIBX52IO_AXIS_MAX];
    for (int i = 0; i < LIBX52IO_AXIS_MAX; i++) {
        denoise_mask[i] = 1234;
    }

    int rc = evtest_init_denoise_masks(NULL, denoise_mask, false);
    assert_int_equal(rc, LIBX52IO_SUCCESS);
    for (int i = 0; i < LIBX52IO_AXIS_MAX; i++) {
        assert_int_equal(denoise_mask[i], 0);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_format_events_no_change),
        cmocka_unit_test(test_format_events_button_change),
        cmocka_unit_test(test_format_events_axis_change_denoise_disabled),
        cmocka_unit_test(test_format_events_axis_change_denoise_filtered),
        cmocka_unit_test(test_format_events_axis_and_button_combined),
        cmocka_unit_test(test_init_denoise_masks_disabled),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
