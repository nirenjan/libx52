/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libx52.h"
#include "x52_test_common.h"

#define TEST_LED(name, state) do { \
    printf(_("LED %s - %s\n"), #name, #state); \
    TEST(led_state, LIBX52_LED_ ## name, LIBX52_LED_STATE_ ## state); \
    if (!nodelay) usleep(500000); \
} while(0)

#define TEST_LED_MONO(name) do { \
    printf(_("\nTesting LED %s\n"), #name); \
    if (!nodelay) sleep(2); \
    TEST_LED(name, OFF); \
    TEST_LED(name, ON); \
} while(0)

#define TEST_LED_COLOR(name) do {\
    printf(_("\nTesting LED %s\n"), #name); \
    if (!nodelay) sleep(2); \
    TEST_LED(name, OFF); \
    TEST_LED(name, RED); \
    TEST_LED(name, AMBER); \
    TEST_LED(name, GREEN); \
} while(0)

int test_leds(void)
{
    int rc;
    print_banner(_("LEDs"));

    /* Check if the connected device supports individual LED control */
    rc = libx52_check_feature(dev, LIBX52_FEATURE_LED);
    if (rc == LIBX52_ERROR_NOT_SUPPORTED) {
        puts(_("Skipping LED tests since the device does not support LED control"));
        return 0;
    } else if (rc != LIBX52_SUCCESS) {
        return rc;
    }

    puts(_("This cycles the LEDs through all possible states"));

    TEST_LED_MONO(FIRE);
    TEST_LED_COLOR(A);
    TEST_LED_COLOR(B);
    TEST_LED_COLOR(D);
    TEST_LED_COLOR(E);
    TEST_LED_COLOR(T1);
    TEST_LED_COLOR(T2);
    TEST_LED_COLOR(T3);
    TEST_LED_COLOR(POV);
    TEST_LED_COLOR(CLUTCH);
    TEST_LED_MONO(THROTTLE);

    return 0;
}

#define TEST_BLINK_OR_SHIFT(type) do { \
    printf(_("\nTesting %s\n"), #type); \
    if (!nodelay) sleep(1); \
    puts(#type " ON"); \
    TEST(type, 1); \
    if (!nodelay) sleep(2); \
    puts(#type " OFF"); \
    TEST(type, 0); \
} while (0)

int test_blink_n_shift(void)
{
    print_banner(_("Blink & Shift"));
    puts(_("This tests the blink indicator and shift functionality"));

    TEST_BLINK_OR_SHIFT(blink);
    TEST_BLINK_OR_SHIFT(shift);

    return 0;
}
