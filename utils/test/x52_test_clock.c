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
#include <time.h>

#include "libx52.h"
#include "x52_test_common.h"

int test_clock(void)
{
    time_t i;
    time_t start;
    time_t end;

    #define TEST_CLOCK_LOOP(bump) do { \
        for (i = start; i < end; i += bump) { \
            TEST(clock, i, 0); \
            if (!nodelay) usleep(250000); \
        } \
    } while (0)

    print_banner("Clock");
    puts("This tests the clock display");
    
    /* Start from midnight Jan 1 2015 GMT */
    start = 1420070400;

    puts("\nTesting clock time minute display for 90 minutes");
    end = start + 90 * 60;
    TEST(clock_format, LIBX52_CLOCK_1, LIBX52_CLOCK_FORMAT_12HR);
    TEST_CLOCK_LOOP(60);

    end = start + 36 * 60 * 60;
    puts("\nTesting clock time hour display for 36 hours, 12 hour mode");
    TEST_CLOCK_LOOP(3600);

    puts("\nTesting clock time hour display for 36 hours, 24 hour mode");
    TEST(clock_format, LIBX52_CLOCK_1, LIBX52_CLOCK_FORMAT_24HR);
    TEST_CLOCK_LOOP(3600);

    end = start + 31 * 24 * 60 * 60;
    puts("\nTesting clock date display for 31 days, dd-mm-yy");
    TEST(date_format, LIBX52_DATE_FORMAT_DDMMYY);
    TEST_CLOCK_LOOP(86400);

    puts("\nTesting clock date display for 31 days, mm-dd-yy");
    TEST(date_format, LIBX52_DATE_FORMAT_MMDDYY);
    TEST_CLOCK_LOOP(86400);

    puts("\nTesting clock date display for 31 days, yy-mm-dd");
    TEST(date_format, LIBX52_DATE_FORMAT_YYMMDD);
    TEST_CLOCK_LOOP(86400);

    return 0;
}
