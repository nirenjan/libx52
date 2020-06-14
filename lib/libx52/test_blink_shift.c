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

TEST_STRUCT (
    blink_shift_fn func;
    uint8_t state;
    struct ivpair data[2];
)

#define on 1
#define off 0

#define x52_shift_indicator 0xfd
#define x52_shift_on 0x51
#define x52_shift_off 0x50

#define x52_blink_indicator 0xb4
#define x52_blink_on 0x51
#define x52_blink_off 0x50

#define TEST(func, state) { #func "/" #state, libx52_set_ ## func, state, {{ x52_ ## func ## _indicator, x52_ ## func ## _ ## state}, {0, 0}}}

TEST_CASES = {
    TEST(blink, on),
    TEST(blink, off),
    TEST(shift, on),
    TEST(shift, off),
};

TEST_FUNC()
{
    TEST_INIT();
    int rc = (*test.func)(dev, test.state);

    if (rc != LIBX52_SUCCESS) {
        PRINT_FAIL();
        printf("# Expected success, got %d\n", rc);
        return;
    }

    TEST_VERIFY(test.data);
}

TEST_MAIN()
