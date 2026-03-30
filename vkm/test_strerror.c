/*
 * VKM — strerror unit tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "vkm.h"

static void test_strerror(void **state)
{
    (void)state;

    static const char *error_map[VKM_ERROR_MAX] = {
        [VKM_SUCCESS] = "Success",
        [VKM_ERROR_UNKNOWN] = "Unknown error",
        [VKM_ERROR_NOT_READY] = "Not ready",
        [VKM_ERROR_OUT_OF_MEMORY] = "Out of memory",
        [VKM_ERROR_INVALID_PARAM] = "Invalid parameter",
        [VKM_ERROR_NOT_SUPPORTED] = "Not supported",
        [VKM_ERROR_DEV_FAILURE] = "Virtual device failure",
        [VKM_ERROR_EVENT] = "Unable to write event",
        [VKM_ERROR_NO_CHANGE] = "No state change",
    };

    static const char *unknown_fmt = "Unknown error %d";

    char expected[256];

    for (int i = -1; i <= (int)VKM_ERROR_MAX + 1; i++) {
        if (i < 0 || i >= (int)VKM_ERROR_MAX || error_map[i] == NULL) {
            snprintf(expected, sizeof(expected), unknown_fmt, i);
        } else {
            strncpy(expected, error_map[i], sizeof(expected));
        }
        assert_string_equal(expected, vkm_strerror(i));
    }
}

static const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_strerror),
};

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
