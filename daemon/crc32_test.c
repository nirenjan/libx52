/*
 * Saitek X52 Pro MFD & LED driver - CRC-32 unit tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include <daemon/crc32.h>

/* Golden values: Python zlib.crc32(data) & 0xFFFFFFFF */

static void test_init_zero(void **state)
{
    (void)state;
    assert_int_equal((int)x52_crc32_init(), 0);
    assert_int_equal((int)X52_CRC32_INIT, 0);
}

static void test_empty(void **state)
{
    (void)state;
    uint32_t crc = x52_crc32_init();
    crc = x52_crc32_update(crc, NULL, 0);
    assert_true(crc == 0u);
    crc = x52_crc32_update(crc, "", 0);
    assert_true(crc == 0u);
}

static void test_len0_preserves(void **state)
{
    (void)state;
    const char *s = "abc";
    uint32_t crc = x52_crc32_update(0, s, 3);
    uint32_t again = x52_crc32_update(crc, s, 0);
    assert_true(again == crc);
}

static void test_string_123456789(void **state)
{
    (void)state;
    static const char s[] = "123456789";
    uint32_t crc = x52_crc32_update(0, s, sizeof(s) - 1u);
    assert_true(crc == 0xcbf43926u);
}

static void test_bytes_0_to_255(void **state)
{
    (void)state;
    unsigned char buf[256];
    for (unsigned i = 0; i < 256; i++) {
        buf[i] = (unsigned char)i;
    }
    uint32_t crc = x52_crc32_update(0, buf, sizeof(buf));
    assert_true(crc == 0x29058c73u);
}

static void test_incremental_matches_one_shot(void **state)
{
    (void)state;
    static const char s[] = "123456789";
    uint32_t a = x52_crc32_update(0, s, sizeof(s) - 1u);

    uint32_t b = x52_crc32_init();
    b = x52_crc32_update(b, s, 3);
    b = x52_crc32_update(b, s + 3, 3);
    b = x52_crc32_update(b, s + 6, 3);

    assert_true(b == a);
}

static void test_chaining_second_segment(void **state)
{
    (void)state;
    static const char h[] = "hello";
    static const char w[] = "world";
    static const char hw[] = "helloworld";

    uint32_t c = x52_crc32_update(0, h, sizeof(h) - 1u);
    c = x52_crc32_update(c, w, sizeof(w) - 1u);

    uint32_t whole = x52_crc32_update(0, hw, sizeof(hw) - 1u);
    assert_true(c == whole);
    assert_true(c == 0xf9eb20adu);
}

static void test_one_byte_at_a_time(void **state)
{
    (void)state;
    static const char s[] = "123456789";
    uint32_t expect = x52_crc32_update(0, s, sizeof(s) - 1u);
    uint32_t crc = x52_crc32_init();
    for (size_t i = 0; i < sizeof(s) - 1u; i++) {
        crc = x52_crc32_update(crc, s + i, 1);
    }
    assert_true(crc == expect);
}

static void test_x52_crc32_alias(void **state)
{
    (void)state;
    static const char s[] = "123456789";
    uint32_t u = x52_crc32_update(0, s, sizeof(s) - 1u);
    uint32_t v = x52_crc32(0, s, sizeof(s) - 1u);
    assert_true(u == v);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_zero),
        cmocka_unit_test(test_empty),
        cmocka_unit_test(test_len0_preserves),
        cmocka_unit_test(test_string_123456789),
        cmocka_unit_test(test_bytes_0_to_255),
        cmocka_unit_test(test_incremental_matches_one_shot),
        cmocka_unit_test(test_chaining_second_segment),
        cmocka_unit_test(test_one_byte_at_a_time),
        cmocka_unit_test(test_x52_crc32_alias),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
