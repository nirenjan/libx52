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

#define TEST_BRIGHTNESS(mfd, value) TEST(brightness, mfd, value)

int test_brightness(void)
{
    int i;

    print_banner("Brightness");

    puts("This test cycles through the MFD and LED brightness scales");

    fputs("\n    |+---+---+---+---+---+---+---+---+|", stdout);
    fputs("\nMFD: ", stdout);
    for (i = 0; i < 129; i++) {
        if (!(i & 3)) fputs("#", stdout);
        fflush(stdout);
        TEST_BRIGHTNESS(1, i);
        if (!nodelay) usleep(250000);
    }
    fputs("\nLED: ", stdout);
    for (i = 0; i < 129; i++) {
        if (!(i & 3)) fputs("#", stdout);
        fflush(stdout);
        TEST_BRIGHTNESS(0, i);
        if (!nodelay) usleep(250000);
    }

    fputs("\n\n", stdout);

    return 0;
}

int test_mfd_text(void)
{
    int i;
    int j;
    char str[32];

    print_banner("MFD text");
    puts("This test tests the character displays of the MFD\n");

    for (i = 0; i < 256; i += 16) {
        j = snprintf(str, 16, "0x%02x - 0x%02x", i, i + 0xf);
        printf("Writing characters %s\n", str);
        TEST(text, 0, str, j);

        memset(str, ' ', 32);
        for (j = 0; j < 16; j++) {
            str[j << 1] = i + j;        
        }

        TEST(text, 1, str, 16);
        TEST(text, 2, str + 16, 16);

        if (!nodelay) sleep(2);
    } 

    return 0;
}

int test_mfd_display(void)
{
    int i;
    char str[16];

    print_banner("MFD display");
    puts("This test checks if the display elements can display all characters");
    puts("You should see the display cycling through each character, with every");
    puts("cell displaying the same character\n");

    for (i = 0; i < 256; i++) {
        printf("Testing character 0x%02x...", i);

        memset(str, i, 16);

        TEST(text, 0, str, 16);
        TEST(text, 1, str, 16);
        TEST(text, 2, str, 16);

        puts("OK");
        if (!nodelay) usleep(500000);
    } 

    return 0;
}
