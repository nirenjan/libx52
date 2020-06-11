/*
 * Saitek X52 Pro MFD & LED driver
 * Test program for validating LED sets
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "x52_common.h"
#include "test_common.h"
#include "x52_commands.h"

struct test_case {
    const char *test_case_id;
    libx52_led_id led_id;
    libx52_led_state state;
    int retval;
    struct ivpair data[3];
};

#define X52_LED_CMD 0xb8
#define UNSUPPORTED(led, state) { #led "/" #state " unsupported", LIBX52_LED_ ## led, LIBX52_LED_STATE_ ## state, LIBX52_ERROR_NOT_SUPPORTED}
#define OFF_MONO(led) { #led "/Off", LIBX52_LED_## led, LIBX52_LED_STATE_OFF, LIBX52_SUCCESS, {{X52_LED_CMD, ((LIBX52_LED_ ## led) << 8)}, {0, 0}}}
#define ON(led) { #led "/On", LIBX52_LED_## led, LIBX52_LED_STATE_ON, LIBX52_SUCCESS, {{X52_LED_CMD, ((LIBX52_LED_ ## led) << 8) | 1}, {0, 0}}}
#define OFF_COLOR(led) { #led "/Off", LIBX52_LED_## led, LIBX52_LED_STATE_OFF, LIBX52_SUCCESS, {{X52_LED_CMD, ((LIBX52_LED_ ## led + 0) << 8)}, {X52_LED_CMD, ((LIBX52_LED_ ## led + 1) << 8)}, {0, 0}}}
#define RED(led) { #led "/Red", LIBX52_LED_## led, LIBX52_LED_STATE_RED, LIBX52_SUCCESS, {{X52_LED_CMD, ((LIBX52_LED_ ## led + 0) << 8) | 1}, {X52_LED_CMD, ((LIBX52_LED_ ## led + 1) << 8) | 0}, {0, 0}}}
#define AMBER(led) { #led "/Amber", LIBX52_LED_## led, LIBX52_LED_STATE_AMBER, LIBX52_SUCCESS, {{X52_LED_CMD, ((LIBX52_LED_ ## led + 0) << 8) | 1}, {X52_LED_CMD, ((LIBX52_LED_ ## led + 1) << 8) | 1}, {0, 0}}}
#define GREEN(led) { #led "/Green", LIBX52_LED_## led, LIBX52_LED_STATE_GREEN, LIBX52_SUCCESS, {{X52_LED_CMD, ((LIBX52_LED_ ## led + 0) << 8) | 0}, {X52_LED_CMD, ((LIBX52_LED_ ## led + 1) << 8) | 1}, {0, 0}}}

const struct test_case test_cases[] = {
    OFF_MONO(FIRE),
    ON(FIRE),
    UNSUPPORTED(FIRE, RED),
    UNSUPPORTED(FIRE, AMBER),
    UNSUPPORTED(FIRE, GREEN),

    OFF_COLOR(A),
    UNSUPPORTED(A, ON),
    RED(A),
    AMBER(A),
    GREEN(A),

    OFF_COLOR(B),
    UNSUPPORTED(B, ON),
    RED(B),
    AMBER(B),
    GREEN(B),

    OFF_COLOR(D),
    UNSUPPORTED(D, ON),
    RED(D),
    AMBER(D),
    GREEN(D),

    OFF_COLOR(E),
    UNSUPPORTED(E, ON),
    RED(E),
    AMBER(E),
    GREEN(E),

    OFF_COLOR(T1),
    UNSUPPORTED(T1, ON),
    RED(T1),
    AMBER(T1),
    GREEN(T1),

    OFF_COLOR(T2),
    UNSUPPORTED(T2, ON),
    RED(T2),
    AMBER(T2),
    GREEN(T2),

    OFF_COLOR(T3),
    UNSUPPORTED(T3, ON),
    RED(T3),
    AMBER(T3),
    GREEN(T3),

    OFF_COLOR(POV),
    UNSUPPORTED(POV, ON),
    RED(POV),
    AMBER(POV),
    GREEN(POV),

    OFF_COLOR(CLUTCH),
    UNSUPPORTED(CLUTCH, ON),
    RED(CLUTCH),
    AMBER(CLUTCH),
    GREEN(CLUTCH),

    OFF_MONO(THROTTLE),
    ON(THROTTLE),
    UNSUPPORTED(THROTTLE, RED),
    UNSUPPORTED(THROTTLE, AMBER),
    UNSUPPORTED(THROTTLE, GREEN),
};

#define TC_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

void run_test(int tc_id)
{
    struct libx52_device *dev = x52_test_init();

    struct test_case test = test_cases[tc_id];

    /* Set the X52Pro flag in dev->flags, otherwise libx52_set_led_state will
     * always return not supported
     */
    dev->flags = 1;

    #define PRINT_FAIL() printf("not ok %d %s\n", tc_id+1, test.test_case_id)
    #define PRINT_PASS() printf("ok %d %s\n", tc_id+1, test.test_case_id)
    int rc = libx52_set_led_state(dev, test.led_id, test.state);

    if (rc != test.retval) {
        PRINT_FAIL();
        printf("# Expected retval %d, got %d\n", test.retval, rc);
        return;
    }

    rc = libx52_update(dev);
    if (rc != LIBX52_SUCCESS) {
        PRINT_FAIL();
        printf("# libx52_update failed, rc = %d\n", rc);
        return;
    }

    if (!x52_test_assert_expected(dev, test.data)) {
        PRINT_FAIL();
        x52_test_print_diagnostics();
        return;
    }

    PRINT_PASS();
    x52_test_cleanup(dev);
}

int main()
{
    int i;

    printf("1..%ld\n", TC_COUNT);
    for (i = 0; i < TC_COUNT; i++) {
        run_test(i);
    }
}
