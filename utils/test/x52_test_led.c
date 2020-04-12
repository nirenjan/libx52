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
    puts("LED " #name " - " #state); \
    TEST(led_state, LIBX52_LED_ ## name, LIBX52_LED_STATE_ ## state); \
    if (!nodelay) usleep(500000); \
} while(0)

#define TEST_LED_MONO(name) do { \
    puts("\nTesting LED " #name); \
    if (!nodelay) sleep(2); \
    TEST_LED(name, OFF); \
    TEST_LED(name, ON); \
} while(0)

#define TEST_LED_COLOR(name) do {\
    puts("\nTesting LED " #name); \
    if (!nodelay) sleep(2); \
    TEST_LED(name, OFF); \
    TEST_LED(name, RED); \
    TEST_LED(name, AMBER); \
    TEST_LED(name, GREEN); \
} while(0)

int test_leds(void)
{
    print_banner("LEDs");
    puts("This cycles the LEDs through all possible states");

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
    puts("\nTesting " #type); \
    if (!nodelay) sleep(1); \
    puts(#type " ON"); \
    TEST(type, 1); \
    if (!nodelay) sleep(2); \
    puts(#type " OFF"); \
    TEST(type, 0); \
} while (0)

int test_blink_n_shift(void)
{
    print_banner("Blink & Shift");
    puts("This tests the blink indicator and shift functionality");

    TEST_BLINK_OR_SHIFT(blink);
    TEST_BLINK_OR_SHIFT(shift);

    return 0;
}
