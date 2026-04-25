/*
 * libx52util MFD scroll tests (cmocka, TAP output)
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <errno.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cmocka.h>
#include <libx52/libx52util.h>

#define MFD_LINE 16u

static void utf8_to_mfd(const char *utf8, uint8_t *out, size_t *out_len)
{
    *out_len = 256;
    (void)libx52util_convert_utf8_string((const uint8_t *)utf8, out, out_len);
}

static bool frame_all_spaces(const uint8_t *d)
{
    for (size_t i = 0; i < MFD_LINE; i++) {
        if (d[i] != (uint8_t)' ') {
            return false;
        }
    }
    return true;
}

static void test_scroll_new_null_state(void **state)
{
    (void)state;
    assert_int_equal(-EINVAL,
                     libx52util_scroll_new(NULL, (const uint8_t *)"x",
                                           LIBX52UTIL_SCROLL_NONE));
}

static void test_scroll_new_null_utf8(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    assert_int_equal(-EINVAL,
                     libx52util_scroll_new(&st, NULL, LIBX52UTIL_SCROLL_NONE));
    assert_null(st);
}

static void test_scroll_next_null_state(void **state)
{
    (void)state;
    uint8_t disp[MFD_LINE];

    memset(disp, 0xAA, sizeof(disp));
    assert_int_equal(-EINVAL, libx52util_scroll_next(NULL, disp));
}

static void test_scroll_next_null_display(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;

    assert_int_equal(0, libx52util_scroll_new(&st, (const uint8_t *)"z",
                                                LIBX52UTIL_SCROLL_NONE));
    assert_non_null(st);
    assert_int_equal(-EINVAL, libx52util_scroll_next(st, NULL));
    libx52util_scroll_free(&st);
}

static void test_scroll_reset_null(void **state)
{
    (void)state;
    assert_int_equal(-EINVAL, libx52util_scroll_reset(NULL));
}

static void test_short_string_first_then_eagain(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];

    assert_int_equal(0, libx52util_scroll_new(&st, (const uint8_t *)"AB",
                                              LIBX52UTIL_SCROLL_NONE));
    assert_non_null(st);
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    assert_int_equal(-EAGAIN, libx52util_scroll_next(st, disp));
    libx52util_scroll_free(&st);
}

static void test_long_string_wrap(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t first[MFD_LINE];
    uint8_t disp[MFD_LINE];
    uint8_t disp2[MFD_LINE];

    assert_int_equal(
        0, libx52util_scroll_new(&st, (const uint8_t *)"01234567890123456",
                                 LIBX52UTIL_SCROLL_NONE));
    assert_non_null(st);
    assert_int_equal(0, libx52util_scroll_next(st, first));
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    assert_int_equal(0, libx52util_scroll_next(st, disp2));
    assert_memory_equal(first, disp2, MFD_LINE);
    libx52util_scroll_free(&st);
}

static void test_identical_glyph_run(void **state)
{
    (void)state;
    char buf[32];
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];
    uint8_t disp2[MFD_LINE];
    int rc;

    memset(buf, 'A', sizeof(buf));
    buf[17] = '\0';
    assert_int_equal(0, libx52util_scroll_new(&st, (const uint8_t *)buf,
                                              LIBX52UTIL_SCROLL_NONE));
    assert_non_null(st);
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    memcpy(disp2, disp, sizeof(disp));
    rc = libx52util_scroll_next(st, disp);
    assert_int_equal(-EAGAIN, rc);
    assert_memory_equal(disp, disp2, MFD_LINE);
    libx52util_scroll_free(&st);
}

static void test_scroll_in_first_all_spaces(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];

    assert_int_equal(
        0,
        libx52util_scroll_new(&st, (const uint8_t *)"01234567890123456789",
                              LIBX52UTIL_SCROLL_IN));
    assert_non_null(st);
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    assert_true(frame_all_spaces(disp));
    libx52util_scroll_free(&st);
}

static void test_scroll_out_eventually_all_spaces(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];
    bool saw_all_spaces = false;
    int rc;
    int i;

    assert_int_equal(
        0, libx52util_scroll_new(&st, (const uint8_t *)"01234567890123456",
                                 LIBX52UTIL_SCROLL_OUT));
    assert_non_null(st);
    for (i = 0; i < 64 && !saw_all_spaces; i++) {
        rc = libx52util_scroll_next(st, disp);
        if (rc == 0 && frame_all_spaces(disp)) {
            saw_all_spaces = true;
        } else if (rc == -EAGAIN) {
            continue;
        } else if (rc != 0) {
            break;
        }
    }
    assert_true(saw_all_spaces);
    libx52util_scroll_free(&st);
}

static void test_reset_rewinds(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t first[MFD_LINE];
    uint8_t disp[MFD_LINE];

    assert_int_equal(
        0, libx52util_scroll_new(&st, (const uint8_t *)"01234567890123456",
                                 LIBX52UTIL_SCROLL_NONE));
    assert_non_null(st);
    assert_int_equal(0, libx52util_scroll_next(st, first));
    (void)libx52util_scroll_next(st, disp);
    (void)libx52util_scroll_next(st, disp);
    assert_int_equal(0, libx52util_scroll_reset(st));
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    assert_memory_equal(first, disp, MFD_LINE);
    libx52util_scroll_free(&st);
}

static void test_single_pass_clamp_then_reset(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];
    int rc;
    int zeros = 0;
    int steps = 0;
    int i;

    assert_int_equal(
        0, libx52util_scroll_new(&st, (const uint8_t *)"01234567890123456",
                                 LIBX52UTIL_SCROLL_SINGLE_PASS));
    assert_non_null(st);
    while (steps < 4096) {
        rc = libx52util_scroll_next(st, disp);
        steps++;
        if (rc == 0) {
            zeros++;
        }
        if (zeros > 2 && rc == -EAGAIN) {
            int eagain_streak = 0;
            for (i = 0; i < 8; i++) {
                uint8_t tmp[MFD_LINE];

                memcpy(tmp, disp, sizeof(tmp));
                rc = libx52util_scroll_next(st, disp);
                steps++;
                if (rc == -EAGAIN && memcmp(disp, tmp, MFD_LINE) == 0) {
                    eagain_streak++;
                }
            }
            if (eagain_streak >= 8) {
                break;
            }
        }
    }
    assert_int_equal(0, libx52util_scroll_reset(st));
    assert_true(zeros > 0);
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    libx52util_scroll_free(&st);
}

static void test_e2big_truncated_prefix(void **state)
{
    (void)state;
    char big[512];
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];
    uint8_t mfd[256];
    size_t mfd_len;
    int rc;

    memset(big, 'B', sizeof(big) - 1);
    big[sizeof(big) - 1] = '\0';
    rc = libx52util_scroll_new(&st, (const uint8_t *)big, LIBX52UTIL_SCROLL_NONE);
    assert_int_equal(-E2BIG, rc);
    assert_non_null(st);
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    mfd_len = sizeof(mfd);
    utf8_to_mfd(big, mfd, &mfd_len);
    assert_true((size_t)mfd_len >= MFD_LINE);
    assert_memory_equal(mfd, disp, MFD_LINE);
    libx52util_scroll_free(&st);
    assert_null(st);
}

static void test_scroll_free_nulls_state(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;

    assert_int_equal(0, libx52util_scroll_new(&st, (const uint8_t *)"q",
                                                LIBX52UTIL_SCROLL_NONE));
    assert_non_null(st);
    libx52util_scroll_free(&st);
    assert_null(st);
}

static void test_ltr_first_frame_trailing(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;
    uint8_t disp[MFD_LINE];
    uint8_t mfd[256];
    size_t mfd_len;

    assert_int_equal(
        0, libx52util_scroll_new(&st, (const uint8_t *)"01234567890123456",
                                 LIBX52UTIL_SCROLL_LTR));
    assert_non_null(st);
    mfd_len = sizeof(mfd);
    utf8_to_mfd("01234567890123456", mfd, &mfd_len);
    assert_true(mfd_len >= 17);
    assert_int_equal(0, libx52util_scroll_next(st, disp));
    assert_memory_equal(mfd + (mfd_len - MFD_LINE), disp, MFD_LINE);
    libx52util_scroll_free(&st);
}

static void test_scroll_free_defensive_null(void **state)
{
    (void)state;
    libx52util_scroll_state *st = NULL;

    libx52util_scroll_free(NULL);
    libx52util_scroll_free(&st);
    assert_null(st);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_scroll_new_null_state),
        cmocka_unit_test(test_scroll_new_null_utf8),
        cmocka_unit_test(test_scroll_next_null_state),
        cmocka_unit_test(test_scroll_next_null_display),
        cmocka_unit_test(test_scroll_reset_null),
        cmocka_unit_test(test_short_string_first_then_eagain),
        cmocka_unit_test(test_long_string_wrap),
        cmocka_unit_test(test_identical_glyph_run),
        cmocka_unit_test(test_scroll_in_first_all_spaces),
        cmocka_unit_test(test_scroll_out_eventually_all_spaces),
        cmocka_unit_test(test_reset_rewinds),
        cmocka_unit_test(test_single_pass_clamp_then_reset),
        cmocka_unit_test(test_e2big_truncated_prefix),
        cmocka_unit_test(test_scroll_free_nulls_state),
        cmocka_unit_test(test_ltr_first_frame_trailing),
        cmocka_unit_test(test_scroll_free_defensive_null),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
