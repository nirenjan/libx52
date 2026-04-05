/*
 * Saitek X52 Pro MFD & LED driver - layout HID usage allowlist tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <daemon/layout_usage_allowlist.h>
#include <vkm/vkm.h>

static void test_allows_main_block(void **state)
{
    (void)state;

    assert_true(x52_layout_usage_key_allowed(VKM_KEY_A));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_Z));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_1));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_0));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_SPACE));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_CAPS_LOCK));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_INTL_BACKSLASH));
    assert_true(x52_layout_usage_key_allowed(VKM_KEY_NONUS_HASH));
}

static void test_rejects_disallowed(void **state)
{
    (void)state;

    assert_false(x52_layout_usage_key_allowed(0));
    assert_false(x52_layout_usage_key_allowed(VKM_KEY_F1));
    assert_false(x52_layout_usage_key_allowed(VKM_KEY_LEFT_CTRL));
    assert_false(x52_layout_usage_key_allowed(VKM_KEY_KEYPAD_1));
    assert_false(x52_layout_usage_key_allowed(0x03));
    assert_false(x52_layout_usage_key_allowed(0x3A));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_allows_main_block),
        cmocka_unit_test(test_rejects_disallowed),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
