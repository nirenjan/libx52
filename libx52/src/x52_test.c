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

#define RUN_TEST(tst) rc = test_ ## tst (); if (rc) break;

int main()
{
    int rc = 0;

    printf("x52test is a suite of tests to write to the X52 Pro device\n");
    printf("and test the extra functionality available in the LEDs and MFD\n");
    printf("\nThese tests take quite some time, press Enter to begin the test");

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
