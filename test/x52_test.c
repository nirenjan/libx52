/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "libx52.h"
#include "x52_test_common.h"
libx52_device *dev;
int test_exit;

void test_cleanup(void)
{
    libx52_set_text(dev, 0, " Saitek X52 Pro ", 16);
    libx52_set_text(dev, 1, "     Flight     ", 16);
    libx52_set_text(dev, 2, " Control System ", 16);

    libx52_set_led_state(dev, LIBX52_LED_FIRE, LIBX52_LED_STATE_ON);
    libx52_set_led_state(dev, LIBX52_LED_THROTTLE, LIBX52_LED_STATE_ON);
    libx52_set_led_state(dev, LIBX52_LED_A, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_B, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_D, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_E, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_T1, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_T2, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_T3, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_POV, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_CLUTCH, LIBX52_LED_STATE_GREEN);

    libx52_set_blink(dev, 0);
    libx52_set_shift(dev, 0);

    libx52_set_clock(dev, time(NULL), 1); // Display local time
    libx52_set_clock_timezone(dev, LIBX52_CLOCK_2, 0); // GMT
    libx52_set_clock_timezone(dev, LIBX52_CLOCK_3, 330); // IST

    libx52_set_clock_format(dev, LIBX52_CLOCK_1, LIBX52_CLOCK_FORMAT_24HR);
    libx52_set_clock_format(dev, LIBX52_CLOCK_2, LIBX52_CLOCK_FORMAT_24HR);
    libx52_set_clock_format(dev, LIBX52_CLOCK_3, LIBX52_CLOCK_FORMAT_24HR);

    libx52_set_date_format(dev, LIBX52_DATE_FORMAT_MMDDYY);

    libx52_set_brightness(dev, 1, 128);
    libx52_set_brightness(dev, 0, 128);

    libx52_update(dev);
}

void print_banner(const char *message)
{
    size_t len = strlen(message);
    int i;

    putchar('\n');
    putchar('\t');
    for (i = 0; i < len + 2; i++) {
        putchar('=');
    }
    putchar('\n');

    putchar('\t');
    putchar(' ');
    puts(message);

    putchar('\t');
    for (i = 0; i < len + 2; i++) {
        putchar('=');
    }
    putchar('\n');
}

static void signal_handler(int sig)
{
    test_exit = sig;
}

#define TEST_brightness     (1 << 0)
#define TEST_leds           (1 << 1)
#define TEST_mfd_text       (1 << 2)
#define TEST_mfd_display    (1 << 3)
#define TEST_blink_n_shift  (1 << 4)
#define TEST_clock          (1 << 5)

#define TEST_ALL (TEST_brightness | TEST_leds | TEST_mfd_text | TEST_blink_n_shift | TEST_clock | TEST_mfd_display)

static int run_tests(int test_set)
{
#define RUN_TEST(tst) if (test_set & TEST_ ## tst) { rc = test_ ## tst (); if (rc) break; }

    int rc = 0;

    puts("x52test is a suite of tests to write to the X52 Pro device");
    puts("and test the extra functionality available in the LEDs and MFD\n");

    if (test_set == TEST_ALL) {
        puts("These tests take roughly 6 minutes to run");
    }
    puts("Press Enter to begin the tests, press Ctrl-C to abort anytime");

    getc(stdin);

    dev = libx52_init();
    if (dev == NULL) {
        fprintf(stderr, "Unable to connect to X52 Pro Joystick!\n");
        return 1;
    }

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    test_exit = 0;

    do {
        RUN_TEST(brightness)
        RUN_TEST(leds)
        RUN_TEST(mfd_text)
        RUN_TEST(mfd_display)
        RUN_TEST(blink_n_shift)
        RUN_TEST(clock)
    } while (0);

    if (rc > 0) {
        fprintf(stderr, "Received %s signal, quitting...\n", strsignal(rc));
    } else if (rc < 0) {
        fprintf(stderr, "Got error %s\n", strerror(-rc));
    } else {
        puts("All tests completed successfully");
    }

    if (rc >= 0) test_cleanup();
    libx52_exit(dev);
    return 0;
}

void usage(void)
{
    puts("These are the available tests with a description and");
    puts("approximate runtime. Not specifying any tests will run");
    puts("all the tests\n");

    puts("List of tests:");
    puts("==============");
    puts("\tbri\tTest brightness scale (~ 1m)");
    puts("\tled\tTest LED states (~ 45s)");
    puts("\tmfd1\tTest MFD string display (~ 30s)");
    puts("\tmfd2\tTest MFD displays all characters (~ 2m 15s)");
    puts("\tblink\tTest the blink and shift commands (< 10s)");
    puts("\tclock\tTest the clock commands (~ 1m)");
    puts("");
}

int main(int argc, char **argv)
{
    int test_list;
    int i;

    /* Usage: x52test [list of tests] */
    if (argc == 1) {
        /* Run all tests, if none specified */
        test_list = TEST_ALL;
    } else {
        /* Initialize the test list to run no tests, the commands
         * will enable the selective tests
         */
        test_list = 0;
    }

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "help") ||
            !strcmp(argv[i], "--help")) {

            /* Display help string and exit */
            printf("Usage: %s [list of tests]\n\n", argv[0]);
            usage();
            return 0;
        } else if (!strcmp(argv[i], "bri")) {
            test_list |= TEST_brightness;
        } else if (!strcmp(argv[i], "led")) {
            test_list |= TEST_leds;
        } else if (!strcmp(argv[i], "mfd1")) {
            test_list |= TEST_mfd_text;
        } else if (!strcmp(argv[i], "mfd2")) {
            test_list |= TEST_mfd_display;
        } else if (!strcmp(argv[i], "blink")) {
            test_list |= TEST_blink_n_shift;
        } else if (!strcmp(argv[i], "clock")) {
            test_list |= TEST_clock;
        } else {
            printf("Unrecognized test identifier: %s\n\n", argv[i]);
            usage();
            return 1;
        }
    }

    if (test_list) {
        i = run_tests(test_list);
    } else {
        puts("Not running any tests");
    }

    return i;
}
