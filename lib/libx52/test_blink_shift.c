/*
 * Saitek X52 Pro MFD & LED driver
 * Test program for validating blink and shift functionality
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
#include "x52_commands.h"

typedef int (*blink_shift_fn)(libx52_device *, uint8_t);

struct test_case {
    const char *test_case_id;
    blink_shift_fn func;
    uint8_t state;
    struct ivpair data[2];
};

#define on 1
#define off 0

#define x52_shift_indicator 0xfd
#define x52_shift_on 0x51
#define x52_shift_off 0x50

#define x52_blink_indicator 0xb4
#define x52_blink_on 0x51
#define x52_blink_off 0x50

#define TEST(func, state) { #func "/" #state, libx52_set_ ## func, state, {{ x52_ ## func ## _indicator, x52_ ## func ## _ ## state}, {0, 0}}}

const struct test_case test_cases[] = {
    TEST(blink, on),
    TEST(blink, off),
    TEST(shift, on),
    TEST(shift, off),
};

#define TC_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

void run_test(int tc_id)
{
    struct libx52_device *dev = x52_test_init();

    struct test_case test = test_cases[tc_id];

    #define PRINT_FAIL() printf("not ok %d %s\n", tc_id+1, test.test_case_id)
    #define PRINT_PASS() printf("ok %d %s\n", tc_id+1, test.test_case_id)
    int rc = (*test.func)(dev, test.state);

    if (rc != LIBX52_SUCCESS) {
        PRINT_FAIL();
        printf("# Expected success, got %d\n", rc);
        return;
    }

    rc = libx52_update(dev);
    if (rc != LIBX52_SUCCESS) {
        PRINT_FAIL();
        printf("# libx52_update failed, rc = %d\n", rc);
        return;
    }

    if (!x52_test_assert_expected(dev, test.data)) {
        PRINT_FAIL();
        x52_test_print_diagnostics();
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
