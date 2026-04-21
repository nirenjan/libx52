/*
 * Saitek X52 Pro MFD & LED driver - Event test axis denoise unit tests
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

#include "ev_denoise.h"

static void test_mask_small_max_all_bits(void **state)
{
    (void)state;
    /* (max >> 6) == 0 => mask is ~0 => -1 as int32_t */
    assert_int_equal(evtest_axis_denoise_mask(0), -1);
    assert_int_equal(evtest_axis_denoise_mask(63), -1);
}

static void test_mask_large_max_clears_low_bits(void **state)
{
    (void)state;
    int32_t max = 1023;
    int32_t mask = evtest_axis_denoise_mask(max);

    /* 1023 >> 6 == 15; ~15 as int32_t is -16 (low four bits clear in mask) */
    assert_int_equal(mask, (int32_t) ~15);
    assert_int_not_equal(mask, -1);
    /* Noise within cleared low bits should not matter when masked */
    assert_int_equal(100 & mask, 101 & mask);
}

/*
 * (max >> 6) is 0 for max in [0, 63] and becomes 1 at max == 64, so the mask
 * flips from all bits set (-1) to clearing the least significant bit (-2).
 */
static void test_mask_boundary_63_vs_64(void **state)
{
    (void)state;

    assert_int_equal(evtest_axis_denoise_mask(63), -1);
    assert_int_equal(evtest_axis_denoise_mask(64), -2);
    assert_int_equal(evtest_axis_denoise_mask(64), (int32_t) ~(64 >> 6));
}

static void test_axis_changed_mask_boundary_lsb_noise(void **state)
{
    (void)state;
    int32_t mask63 = evtest_axis_denoise_mask(63);
    int32_t mask64 = evtest_axis_denoise_mask(64);

    /* Adjacent values differing only in bit 0: masked equal with -2, not with -1 */
    assert_int_equal(100 & mask64, 101 & mask64);
    assert_int_not_equal(100 & mask63, 101 & mask63);

    assert_false(evtest_axis_changed(100, 101, mask64, true));
    assert_true(evtest_axis_changed(100, 101, mask63, true));
}

static void test_mask_4095_and_axis_changed_low_six_bits(void **state)
{
    (void)state;
    int32_t max = 4095;
    int32_t mask = evtest_axis_denoise_mask(max);

    /* 4095 >> 6 == 63; ~63 as int32_t is -64 (low six bits clear in mask) */
    assert_int_equal(mask, (int32_t) ~63);
    assert_int_equal(mask, -64);

    assert_int_equal(0 & mask, 31 & mask);
    assert_false(evtest_axis_changed(0, 31, mask, true));

    assert_int_not_equal(0 & mask, 64 & mask);
    assert_true(evtest_axis_changed(0, 64, mask, true));
}

static void test_axis_changed_equal_raw(void **state)
{
    (void)state;
    assert_false(evtest_axis_changed(500, 500, -1, true));
    assert_false(evtest_axis_changed(500, 500, -1, false));
}

static void test_axis_changed_unequal_denoise_off(void **state)
{
    (void)state;
    assert_true(evtest_axis_changed(100, 101, -1, false));
}

static void test_axis_changed_equal_after_mask_denoise_on(void **state)
{
    (void)state;
    int32_t mask = evtest_axis_denoise_mask(1023);

    assert_false(evtest_axis_changed(100, 101, mask, true));
}

static void test_axis_changed_still_change_after_mask(void **state)
{
    (void)state;
    int32_t mask = evtest_axis_denoise_mask(1023);

    assert_true(evtest_axis_changed(96, 112, mask, true));
}

static void test_axis_changed_full_mask_any_raw_diff(void **state)
{
    (void)state;
    /* With mask -1, (v & -1) == v, so any raw difference is a change */
    assert_true(evtest_axis_changed(10, 11, -1, true));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_mask_small_max_all_bits),
        cmocka_unit_test(test_mask_large_max_clears_low_bits),
        cmocka_unit_test(test_mask_boundary_63_vs_64),
        cmocka_unit_test(test_axis_changed_mask_boundary_lsb_noise),
        cmocka_unit_test(test_mask_4095_and_axis_changed_low_six_bits),
        cmocka_unit_test(test_axis_changed_equal_raw),
        cmocka_unit_test(test_axis_changed_unequal_denoise_off),
        cmocka_unit_test(test_axis_changed_equal_after_mask_denoise_on),
        cmocka_unit_test(test_axis_changed_still_change_after_mask),
        cmocka_unit_test(test_axis_changed_full_mask_any_raw_diff),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
