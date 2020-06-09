/*
 * Saitek X52 Pro MFD & LED driver
 * Test program for validating offset calculation
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "x52_common.h"

struct test_id {
    const char *test_case_id;
    int offset_primary;
    int offset_secondary;
    uint16_t x52_offset;
};

const struct test_id test_cases[] = {
    {"Etc/UTC+24|Etc/UTC-24", -1440, +1440, 0},
    {"Etc/UTC-24|Etc/UTC+24", +1440, -1440, 0x0400}, // Negative 0
    {"Honolulu|Auckland", -600, +720, 0x0478}, // -2 hours
    {"Auckland|Honolulu", +720, -600, 0x0078}, // +2 hours
    {"Etc/UTC+12|Etc/UTC-14", -720, +840, 0x078}, // +2 hours
    {"Etc/UTC-14|Etc/UTC+12", +840, -720, 0x478}, // -2 hours
    {"PDT|UTC", -420, 0, 0x1a4}, // +7 hours
    {"UTC|PDT", 0, -420, 0x5a4}, // -7 hours
    {"PST|UTC", -480, 0, 0x1e0}, // +8 hours
    {"UTC|PST", 0, -480, 0x5e0}, // -8 hours
    {"Etc/UTC+12|Etc/UTC-12", -720, +720, 0},
    {"Etc/UTC-12|Etc/UTC+12", +720, -720, 0x0400},
};

#define TC_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

void run_test(int tc_id)
{
    struct libx52_device dev;
    memset(&dev, 0, sizeof(dev));

    struct test_id test = test_cases[tc_id];

    dev.timezone[LIBX52_CLOCK_1] = test.offset_primary;
    dev.timezone[LIBX52_CLOCK_2] = test.offset_secondary;

    uint16_t obtained = libx52_calculate_clock_offset(&dev, LIBX52_CLOCK_2, 0);
    if (obtained != test.x52_offset) {
        printf("not ");
    }
    printf("ok %d %s\n", tc_id + 1, test.test_case_id);
    if (obtained != test.x52_offset) {
        // Diagnostics
        printf("# Expected: %04x\n", test.x52_offset);
        printf("# Observed: %04x\n", obtained);
    }
}

int main()
{
    int i;

    printf("1..%ld\n", TC_COUNT);
    for (i = 0; i < TC_COUNT; i++) {
        run_test(i);
    }
}
