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

void x52_test_print_observed_data(libx52_device *dev);
void x52_test_print_expected_data(struct ivpair *data);

#define PRINT_FAIL() printf("not ok %d %s\n", tc_id+1, test.test_case_id)
#define PRINT_PASS() printf("ok %d %s\n", tc_id+1, test.test_case_id)
#define TEST_STRUCT(...) struct test_case { \
    const char *test_case_id; \
    __VA_ARGS__ \
};

#define TEST_CASES const struct test_case test_cases[] 
#define TEST_FUNC() void run_test(int tc_id)
#define TEST_INIT() \
    struct libx52_device *dev = x52_test_init(); \
    struct test_case test = test_cases[tc_id];

#define TEST_VERIFY(data) do { \
    int test_rc; \
    test_rc = libx52_update(dev); \
    if (test_rc != LIBX52_SUCCESS) { \
        PRINT_FAIL(); \
        printf("# libx52_update failed, rc = %d\n", test_rc); \
        return; \
    } \
 \
    if (!x52_test_assert_expected(dev, data)) { \
        PRINT_FAIL(); \
        x52_test_print_diagnostics(); \
        x52_test_print_expected_data(data); \
        x52_test_print_observed_data(dev); \
        return; \
    } \
 \
    PRINT_PASS(); \
    x52_test_cleanup(dev); \
} while(0)

#define TEST_MAIN() int main() { \
    int i; \
    size_t tc_count = sizeof(test_cases) / sizeof(test_cases[0]); \
    printf("1..%lu\n", tc_count); \
    for (i = 0; i < tc_count; i++) { \
        run_test(i); \
    } \
}

#endif // !defined _TEST_COMMON_H
