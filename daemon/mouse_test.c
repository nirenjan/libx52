/*
 * Saitek X52 Pro MFD & LED driver - Mouse driver test harness
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#define PINELOG_MODULE X52D_MOD_MOUSE
#include "pinelog.h"
#include <daemon/config.h>
#include <daemon/constants.h>
#include <daemon/mouse.h>

/* Stub for handler */
void x52d_mouse_thread_control(bool enabled)
{
    function_called();
    check_expected(enabled);
}

static void test_mouse_thread_enabled(void **state)
{
    (void)state;
    expect_function_calls(x52d_mouse_thread_control, 1);
    expect_value(x52d_mouse_thread_control, enabled, true);

    x52d_cfg_set_Mouse_Enabled(true);
}

static void test_mouse_thread_disabled(void **state)
{
    (void)state;
    expect_function_calls(x52d_mouse_thread_control, 1);
    expect_value(x52d_mouse_thread_control, enabled, false);

    x52d_cfg_set_Mouse_Enabled(false);
}

/* The following tests are dependent on the values in mouse.c */
static void test_mouse_speed_negative(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(-1);
    assert_int_equal(mouse_sensitivity, 14);
}

static void test_mouse_speed_0(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(0);
    assert_int_equal(mouse_sensitivity, 14);
}

static void test_mouse_speed_mid_base(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(6);
    assert_int_equal(mouse_sensitivity, 25);
}

static void test_mouse_speed_max_base(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(12);
    assert_int_equal(mouse_sensitivity, 100);
}

static void test_mouse_speed_min_hyper(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(13);
    assert_int_equal(mouse_sensitivity, 125);
}

static void test_mouse_speed_mid_hyper(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(22);
    assert_int_equal(mouse_sensitivity, 350);
}

static void test_mouse_speed_max_hyper(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(32);
    assert_int_equal(mouse_sensitivity, 500);
}

static void test_mouse_speed_above_max(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_Speed(33);
    assert_int_equal(mouse_sensitivity, 500);
}

static void test_mouse_reverse_scroll_enabled(void **state)
{
    (void)state;
    x52d_cfg_set_Mouse_ReverseScroll(true);
    assert_int_equal(mouse_scroll_dir, -1);
}

static void test_mouse_reverse_scroll_disabled(void **state)
{
    (void)state;
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
