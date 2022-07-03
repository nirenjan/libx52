/*
 * Saitek X52 Pro MFD & LED driver - Mouse driver test harness
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#define PINELOG_MODULE X52D_MOD_MOUSE
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

#if defined HAVE_EVDEV
/* Stub for evdev */
void x52d_mouse_evdev_thread_control(bool enabled)
{
    function_called();
    check_expected(enabled);
}
#endif

static void test_mouse_thread_enabled(void **state)
{
    #if defined HAVE_EVDEV
    expect_function_calls(x52d_mouse_evdev_thread_control, 1);
    expect_value(x52d_mouse_evdev_thread_control, enabled, true);
    #endif

    x52d_cfg_set_Mouse_Enabled(true);
}

static void test_mouse_thread_disabled(void **state)
{
    #if defined HAVE_EVDEV
    expect_function_calls(x52d_mouse_evdev_thread_control, 1);
    expect_value(x52d_mouse_evdev_thread_control, enabled, false);
    #endif

    x52d_cfg_set_Mouse_Enabled(false);
}

static void test_mouse_speed_negative(void **state)
{
    int orig_mouse_delay = mouse_delay;
    int orig_mouse_mult = mouse_mult;

    x52d_cfg_set_Mouse_Speed(-1);
    assert_int_equal(mouse_delay, orig_mouse_delay);
    assert_int_equal(mouse_mult, orig_mouse_mult);
}

/* The following tests are dependent on the values in x52d_mouse.c */
static void test_mouse_speed_0(void **state)
{
    x52d_cfg_set_Mouse_Speed(0);
    assert_int_equal(mouse_delay, 70000);
    assert_int_equal(mouse_mult, 4);
}

static void test_mouse_speed_mid_base(void **state)
{
    x52d_cfg_set_Mouse_Speed(6);
    assert_int_equal(mouse_delay, 40000);
    assert_int_equal(mouse_mult, 4);
}

static void test_mouse_speed_max_base(void **state)
{
    x52d_cfg_set_Mouse_Speed(12);
    assert_int_equal(mouse_delay, 10000);
    assert_int_equal(mouse_mult, 4);
}

static void test_mouse_speed_min_hyper(void **state)
{
    x52d_cfg_set_Mouse_Speed(13);
    assert_int_equal(mouse_delay, 10000);
    assert_int_equal(mouse_mult, 5);
}

static void test_mouse_speed_mid_hyper(void **state)
{
    x52d_cfg_set_Mouse_Speed(22);
    assert_int_equal(mouse_delay, 10000);
    assert_int_equal(mouse_mult, 14);
}

static void test_mouse_speed_max_hyper(void **state)
{
    x52d_cfg_set_Mouse_Speed(32);
    assert_int_equal(mouse_delay, 10000);
    assert_int_equal(mouse_mult, 24);
}

static void test_mouse_speed_above_max(void **state)
{
    int orig_mouse_delay = mouse_delay;
    int orig_mouse_mult = mouse_mult;

    x52d_cfg_set_Mouse_Speed(33);
    assert_int_equal(mouse_delay, orig_mouse_delay);
    assert_int_equal(mouse_mult, orig_mouse_mult);
}

static void test_mouse_reverse_scroll_enabled(void **state)
{
    x52d_cfg_set_Mouse_ReverseScroll(true);
    assert_int_equal(mouse_scroll_dir, -1);
}

static void test_mouse_reverse_scroll_disabled(void **state)
{
    x52d_cfg_set_Mouse_ReverseScroll(false);
    assert_int_equal(mouse_scroll_dir, 1);
}

const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_mouse_thread_enabled),
    cmocka_unit_test(test_mouse_thread_disabled),
    cmocka_unit_test(test_mouse_speed_negative),
    cmocka_unit_test(test_mouse_speed_0),
    cmocka_unit_test(test_mouse_speed_mid_base),
    cmocka_unit_test(test_mouse_speed_max_base),
    cmocka_unit_test(test_mouse_speed_min_hyper),
    cmocka_unit_test(test_mouse_speed_mid_hyper),
    cmocka_unit_test(test_mouse_speed_max_hyper),
    cmocka_unit_test(test_mouse_speed_above_max),
    cmocka_unit_test(test_mouse_reverse_scroll_enabled),
    cmocka_unit_test(test_mouse_reverse_scroll_disabled),
};

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);

    return 0;
}
