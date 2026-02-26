/*
 * Saitek X52 Pro MFD & LED driver - Clutch module tests
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "libx52.h"
#include "libx52io.h"

#define PINELOG_MODULE X52D_MOD_CLUTCH
#include "pinelog.h"

#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_device.h"
#include "x52d_profile.h"
#include "x52d_clutch.h"

/* Stub config: return values set by test */
static const char *stub_clutch_enabled = "no";
static const char *stub_clutch_latched = "no";

const char *x52d_config_get(const char *section, const char *key)
{
    if (section != NULL && key != NULL) {
        if (strcasecmp(section, "Profiles") == 0) {
            if (strcasecmp(key, "ClutchEnabled") == 0) return stub_clutch_enabled;
            if (strcasecmp(key, "ClutchLatched") == 0) return stub_clutch_latched;
        }
    }
    return NULL;
}

int x52d_config_set(const char *section, const char *key, const char *value)
{
    (void)section;
    (void)key;
    (void)value;
    return 0;
}

void x52d_profile_init(void)
{
}

/* Stub device: record and optionally check expected */
int x52d_dev_set_led_state(libx52_led_id led, libx52_led_state state)
{
    check_expected(led);
    check_expected(state);
    return 0;
}

int x52d_dev_set_blink(uint8_t state)
{
    (void)check_expected(state);
    return 0;
}

int x52d_dev_set_text(uint8_t line, const char *text, uint8_t length)
{
    (void)line;
    (void)text;
    (void)length;
    return 0;
}

static void test_clutch_disabled_red_led(void **state)
{
    libx52io_report report, prev;

    (void)state;
    stub_clutch_enabled = "no";
    stub_clutch_latched = "no";
    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));

    expect_value(x52d_dev_set_blink, state, 0);
    expect_value(x52d_dev_set_led_state, led, LIBX52_LED_CLUTCH);
    expect_value(x52d_dev_set_led_state, state, LIBX52_LED_STATE_RED);

    assert_false(x52d_clutch_process(&report, &prev));
}

static void test_clutch_enabled_not_engaged_green_led(void **state)
{
    libx52io_report report, prev;

    (void)state;
    stub_clutch_enabled = "yes";
    stub_clutch_latched = "no";
    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.button[LIBX52IO_BTN_CLUTCH] = false;

    expect_value(x52d_dev_set_blink, state, 0);
    expect_value(x52d_dev_set_led_state, led, LIBX52_LED_CLUTCH);
    expect_value(x52d_dev_set_led_state, state, LIBX52_LED_STATE_GREEN);

    assert_false(x52d_clutch_process(&report, &prev));
}

static void test_clutch_engaged_amber_blink(void **state)
{
    libx52io_report report, prev;

    (void)state;
    stub_clutch_enabled = "yes";
    stub_clutch_latched = "yes";
    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));

    /* First: press clutch (no toggle yet for latched) */
    report.button[LIBX52IO_BTN_CLUTCH] = true;
    expect_value(x52d_dev_set_blink, state, 0);
    expect_value(x52d_dev_set_led_state, led, LIBX52_LED_CLUTCH);
    expect_value(x52d_dev_set_led_state, state, LIBX52_LED_STATE_GREEN);
    assert_false(x52d_clutch_process(&report, &prev));

    /* Second: release clutch -> toggle engaged */
    memcpy(&prev, &report, sizeof(report));
    report.button[LIBX52IO_BTN_CLUTCH] = false;
    expect_value(x52d_dev_set_led_state, led, LIBX52_LED_CLUTCH);
    expect_value(x52d_dev_set_led_state, state, LIBX52_LED_STATE_AMBER);
    expect_value(x52d_dev_set_blink, state, 1);
    assert_true(x52d_clutch_process(&report, &prev));
}

static void test_clutch_held_not_latched(void **state)
{
    libx52io_report report, prev;

    (void)state;
    stub_clutch_enabled = "yes";
    stub_clutch_latched = "no";
    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.button[LIBX52IO_BTN_CLUTCH] = true;

    /* Held (not latched): directly to engaged = amber + blink, no prior blink(0) */
    expect_value(x52d_dev_set_led_state, led, LIBX52_LED_CLUTCH);
    expect_value(x52d_dev_set_led_state, state, LIBX52_LED_STATE_AMBER);
    expect_value(x52d_dev_set_blink, state, 1);
    assert_true(x52d_clutch_process(&report, &prev));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_clutch_disabled_red_led),
        cmocka_unit_test(test_clutch_enabled_not_engaged_green_led),
        cmocka_unit_test(test_clutch_engaged_amber_blink),
        cmocka_unit_test(test_clutch_held_not_latched),
    };
    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
