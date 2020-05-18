/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "libx52.h"
#include "x52_test_common.h"
libx52_device *dev;
int test_exit;
bool nodelay;

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
    size_t i;

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
    test_exit = -sig;
}

#define TESTS \
    X(brightness, bri, "Test brightness scale (~ 1m)") \
    X(mfd_text, mfd1, "Test MFD string display (~ 30s)") \
    X(mfd_display, mfd2, "Test MFD displays all characters (~ 2m 15s)") \
    X(blink_n_shift, blink, "Test the blink and shift commands (< 10s)") \
    X(clock, clock, "Test the clock commands (~1m)") \
    X(leds, led, "Test LED states (~ 45s)")

enum {
#define X(en, kw, desc) TEST_BIT_ ## en,
    TESTS
#undef X
};

enum {
#define X(en, kw, desc) TEST_ ## en = (1 << TEST_BIT_ ## en),
    TESTS
#undef X
};

#define X(en, kw, desc) | TEST_ ## en
const int TEST_ALL = 0 TESTS;
#undef X

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

    rc = libx52_init(&dev);
    if (rc != LIBX52_SUCCESS) {
        fprintf(stderr, "Unable to connect to X52 Pro Joystick!\n");
        return 1;
    }

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    test_exit = 0;

    do {
        #define X(en, cmd, desc) RUN_TEST(en)
        TESTS
        #undef X
    } while (0);

    if (rc == LIBX52_SUCCESS) {
        puts("All tests completed successfully");
    } else if (rc > 0) {
        fprintf(stderr, "Got error %s\n", libx52_strerror(rc));
    } else {
        fprintf(stderr, "Received %s signal, quitting...\n", strsignal(-rc));
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

    #define X(en, cmd, desc) puts("\t" #cmd "\t" desc);
    TESTS
    #undef X

    puts("");
}

struct test_map {
    char *cmd;
    int  test_bitmap;
};
const struct test_map tests[] = {
#define X(en, cmd, desc) { #cmd, TEST_ ##en },
    TESTS
#undef X
    { NULL, 0 }
};

int main(int argc, char **argv)
{
    int test_list;
    int i;
    const struct test_map *test;
    int found;

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
        } else {
            found = 0;
            for (test = tests; test->cmd; test++) {
                if (!strcmp(argv[i], test->cmd)) {
                    test_list |= test->test_bitmap;
                    found = 1;
                    break;
                }
            }

            if (found == 0) {
                printf("Unrecognized test identifier: %s\n\n", argv[i]);
                usage();
                return 1;
            }
        }
    }

    /* Initialize the nodelay variable */
    nodelay = (getenv("LD_PRELOAD") != NULL || getenv("NO_DELAY") != NULL);

    if (test_list) {
        i = run_tests(test_list);
    } else {
        puts("Not running any tests");
    }

    return i;
}
