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
#include "test_common.h"

struct test_case {
    const char *test_case_id;
    int offset_primary;
    int offset_secondary;
    libx52_clock_id clock_id;
    uint16_t x52_clock;
    uint16_t x52_offset;
};

#define TEST(id, o1, o23, offs) {id, o1, o23, LIBX52_CLOCK_2, 0xc1, offs}, {id, o1, o23, LIBX52_CLOCK_3, 0xc2, offs}

const struct test_case test_cases[] = {
    TEST("Etc/UTC+24|Etc/UTC-24", -1440, +1440, 0),
    TEST("Etc/UTC-24|Etc/UTC+24", +1440, -1440, 0x0400), // Negative 0
    TEST("Honolulu|Auckland", -600, +720, 0x0478), // -2 hours
    TEST("Auckland|Honolulu", +720, -600, 0x0078), // +2 hours
    TEST("Etc/UTC+12|Etc/UTC-14", -720, +840, 0x078), // +2 hours
    TEST("Etc/UTC-14|Etc/UTC+12", +840, -720, 0x478), // -2 hours
    TEST("PDT|UTC", -420, 0, 0x1a4), // +7 hours
    TEST("UTC|PDT", 0, -420, 0x5a4), // -7 hours
    TEST("PST|UTC", -480, 0, 0x1e0), // +8 hours
    TEST("UTC|PST", 0, -480, 0x5e0), // -8 hours
    TEST("Etc/UTC+12|Etc/UTC-12", -720, +720, 0),
    TEST("Etc/UTC-12|Etc/UTC+12", +720, -720, 0x0400),
};

#define TC_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

void run_test(int tc_id)
{
    struct libx52_device *dev = x52_test_init();
    struct test_case test = test_cases[tc_id];
    int rc;
    struct ivpair data[2] = { 0 };

    #define PRINT_FAIL() printf("not ok %d %s/%d\n", tc_id+1, test.test_case_id, test.clock_id + 1)
    #define PRINT_PASS() printf("ok %d %s/%d\n", tc_id+1, test.test_case_id, test.clock_id + 1)

    dev->timezone[LIBX52_CLOCK_1] = test.offset_primary;
    rc = libx52_set_clock_timezone(dev, test.clock_id, test.offset_secondary);
    if (rc != LIBX52_SUCCESS) {
        PRINT_FAIL();
        printf("# set_clock_timezone failed, rc = %d\n", rc);
        return;
    }

    rc = libx52_update(dev);
    if (rc != LIBX52_SUCCESS) {
        PRINT_FAIL();
        printf("# libx52_update failed, rc = %d\n", rc);
        return;
    }

    data[0].index = test.x52_clock;
    data[0].value = test.x52_offset;

    if (!x52_test_assert_expected(dev, data)) {
        PRINT_FAIL();
        x52_test_print_diagnostics();
        x52_test_print_expected_data(data);
        x52_test_print_observed_data(dev);
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
