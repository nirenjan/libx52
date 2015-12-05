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
#include <string.h>
#include <unistd.h>

#include "libx52.h"
#include "x52_test_common.h"

int test_clock(void)
{
    time_t i;
    time_t start;
    time_t end;

    print_banner("Clock");
    puts("This tests the clock display");
    
    /* Start from midnight Jan 1 2015 GMT */
    start = 1420070400;

    puts("\nTesting clock time minute display for 1 hour increment");
    end = start + 3600;
    TEST(clock_format, LIBX52_CLOCK_1, LIBX52_CLOCK_FORMAT_12HR);
    for (i = start; i < end; i += 60) {
        /* Run all minutes, display in GMT */
        TEST(clock, i, 0);
        usleep(250000);
    }

    puts("\nTesting clock time display for 25 hour increments, 12 hour mode");
    end = start + 86400 + 1440;
    for (i = start; i < end; i += 3600) {
        /* Run all hours, display in GMT */
        TEST(clock, i, 0);
        usleep(250000);
    }

    puts("\nTesting clock time display for 25 hour increments, 24 hour mode");
    TEST(clock_format, LIBX52_CLOCK_1, LIBX52_CLOCK_FORMAT_24HR);
    for (i = start; i < end; i += 3600) {
        /* Run all hours, display in GMT */
        TEST(clock, i, 0);
        usleep(250000);
    }
}
