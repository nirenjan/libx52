/*
 * Saitek X52 Pro MFD & LED driver
 * Common functionality for test programs
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H
#include <stdint.h>
#include <stdbool.h>
#include "x52_common.h"

/* ivpair is a pair of index and value fields that are passed to the
 * test vendor command function.
 */
struct ivpair {
    uint16_t index;
    uint16_t value;
};

#define MAX_SZ 100
struct x52_vendor_data {
    int written;
    struct ivpair data[MAX_SZ];
};

/*
 * Initialize libx52, close any device handles, create a dummy handle
 * and override the vendor command function.
 */
libx52_device *x52_test_init(void);

/*
 * Check if expected data matches with written data. Terminate expected
 * data with a pair of NULLs
 */
bool x52_test_assert_expected(libx52_device *dev, struct ivpair *data);

/* Print diagnostics to screen */
void x52_test_print_diagnostics(void);

/* Cleanup test data */
void x52_test_cleanup(libx52_device *dev);

#endif // !defined _TEST_COMMON_H
