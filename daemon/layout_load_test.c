/*
 * Saitek X52 Pro MFD & LED driver - x52layout loader tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include <daemon/crc32.h>
#include <daemon/layout_format.h>
#include <vkm/vkm.h>

static void write_be16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static void write_be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static uint32_t read_be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | (uint32_t)p[3];
}

/** Patch checksum (offsets 12..15) after building the rest; @p len is full file size. */
static void finalize_crc(uint8_t *buf, size_t len)
{
    uint32_t crc = x52_crc32_init();
    crc = x52_crc32_update(crc, buf, 12u);
    static const uint8_t z[4] = {0, 0, 0, 0};
    crc = x52_crc32_update(crc, z, 4u);
    if (len > 16u) {
        crc = x52_crc32_update(crc, buf + 16u, len - 16u);
    }
    write_be32(buf + 12, crc);
}

/** Python @c zlib.crc32 over @c minimal v1 layout (@c limit=1, name @c "x", empty entry). */
#define X52_LAYOUT_TEST_MINIMAL_ZLIB_CRC32 0xc951bfaau

static void test_load_minimal_lookup(void **state)
{
    (void)state;
    const uint32_t limit = 98u; /* enough for 'a' at 97 */
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u * (size_t)limit;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);

    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "minimal", 8);
    /* chord for 'a': VKM_KEY_A */
    size_t off = X52_LAYOUT_HEADER_BYTES + 2u * 97u;
    buf[off] = 0x02; /* LSHIFT */
    buf[off + 1] = (uint8_t)VKM_KEY_A;
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), 0);
    assert_non_null(L);
    assert_int_equal((int)x52_layout_codepoint_limit(L), (int)limit);

    uint8_t m = 0xff;
    uint8_t u = 0xff;
    assert_true(x52_layout_lookup(L, 97u, &m, &u));
    assert_int_equal((int)m, 0x02);
    assert_int_equal((int)u, (int)VKM_KEY_A);

    m = 0;
    u = 0;
    assert_false(x52_layout_lookup(L, 0u, &m, &u));
    assert_false(x52_layout_lookup(L, limit, &m, &u));

    assert_int_equal((int)x52_layout_flags(L), 0);
    assert_string_equal(x52_layout_name(L), "minimal");
    assert_string_equal(x52_layout_description(L), "");

    x52_layout_free(L);
    free(buf);
}

static void test_reject_bad_checksum(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    write_be32(buf + 12, 0xdeadbeefu);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_layout_crc_matches_python_zlib_minimal(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    finalize_crc(buf, len);
    assert_int_equal((int)read_be32(buf + 12), (int)X52_LAYOUT_TEST_MINIMAL_ZLIB_CRC32);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), 0);
    assert_non_null(L);
    assert_string_equal(x52_layout_name(L), "x");
    x52_layout_free(L);
    free(buf);
}

static void test_reject_tampered_checksum_byte(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    finalize_crc(buf, len);
    buf[12] ^= 0x01u;

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_reject_codepoint_limit_not_big_endian(void **state)
{
    (void)state;
    /* Little-endian uint32_t 1 in the codepoint_limit field: read_be32 → 0x01000000. */
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u * (size_t)limit;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    buf[8] = 0x01u;
    buf[9] = 0x00u;
    buf[10] = 0x00u;
    buf[11] = 0x00u;
    memcpy(buf + 16, "x", 2);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_reject_version_word_not_big_endian_one(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    /* Native little-endian 0x0001 would appear as 01 00 — not BE version 1 (00 01). */
    buf[4] = 0x01u;
    buf[5] = 0x00u;
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_reject_size_mismatch(void **state)
{
    (void)state;
    const uint32_t limit = 4u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u * (size_t)limit;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len - 1u, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_reject_disallowed_usage(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    buf[X52_LAYOUT_HEADER_BYTES + 1u] = 0x3A; /* VKM_KEY_F1 — not in allowlist */
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_metadata_plain(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "us", 3);
    memcpy(buf + 48, "US QWERTY", 10);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), 0);
    assert_non_null(L);
    assert_string_equal(x52_layout_name(L), "us");
    assert_string_equal(x52_layout_description(L), "US QWERTY");
    x52_layout_free(L);
    free(buf);
}

static void test_metadata_truncated_suffix(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6,
               (uint16_t)(X52_LAYOUT_FLAG_NAME_TRUNCATED | X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED));
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "longish", 8);
    memcpy(buf + 48, "desc", 5);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), 0);
    assert_non_null(L);
    assert_int_equal((int)x52_layout_flags(L),
                     (int)(X52_LAYOUT_FLAG_NAME_TRUNCATED | X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED));
    assert_string_equal(x52_layout_name(L), "longish<truncated>");
    assert_string_equal(x52_layout_description(L), "desc<truncated>");
    x52_layout_free(L);
    free(buf);
}

static void test_metadata_name_truncated_flag_only(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, X52_LAYOUT_FLAG_NAME_TRUNCATED);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "nm", 3);
    memcpy(buf + 48, "plain", 6);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), 0);
    assert_non_null(L);
    assert_int_equal((int)x52_layout_flags(L), (int)X52_LAYOUT_FLAG_NAME_TRUNCATED);
    assert_string_equal(x52_layout_name(L), "nm<truncated>");
    assert_string_equal(x52_layout_description(L), "plain");
    x52_layout_free(L);
    free(buf);
}

static void test_reject_unknown_flags(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0x8000);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "x", 2);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_reject_empty_name(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, X52_LAYOUT_FORMAT_VERSION);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

static void test_basename_normalize_and_join(void **state)
{
    (void)state;
    char out[256];
    bool rej;

    x52_layout_normalize_keyboard_basename(NULL, out, sizeof out, &rej);
    assert_false(rej);
    assert_string_equal(out, "us");

    x52_layout_normalize_keyboard_basename("", out, sizeof out, &rej);
    assert_false(rej);
    assert_string_equal(out, "us");

    x52_layout_normalize_keyboard_basename("de", out, sizeof out, &rej);
    assert_false(rej);
    assert_string_equal(out, "de");

    x52_layout_normalize_keyboard_basename("ab_cd-9", out, sizeof out, &rej);
    assert_false(rej);
    assert_string_equal(out, "ab_cd-9");

    x52_layout_normalize_keyboard_basename("../x", out, sizeof out, &rej);
    assert_true(rej);
    assert_string_equal(out, "us");

    x52_layout_normalize_keyboard_basename("a/b", out, sizeof out, &rej);
    assert_true(rej);
    assert_string_equal(out, "us");

    x52_layout_normalize_keyboard_basename("bad name", out, sizeof out, &rej);
    assert_true(rej);
    assert_string_equal(out, "us");

    x52_layout_normalize_keyboard_basename("a\\b", out, sizeof out, &rej);
    assert_true(rej);
    assert_string_equal(out, "us");

    x52_layout_normalize_keyboard_basename("x..y", out, sizeof out, &rej);
    assert_true(rej);
    assert_string_equal(out, "us");

    memset(out, 0, sizeof out);
    x52_layout_normalize_keyboard_basename("almost..", out, sizeof out, &rej);
    assert_true(rej);
    assert_string_equal(out, "us");

    char path[PATH_MAX];
    assert_int_equal(x52_layout_join_file_path(path, sizeof path, "/usr/share", "us"), 0);
    assert_string_equal(path, "/usr/share/x52d/us.x52l");
    assert_int_equal(x52_layout_join_file_path(path, 20, "/usr/share", "us"), ENAMETOOLONG);
}

static void test_reject_version(void **state)
{
    (void)state;
    const uint32_t limit = 1u;
    size_t len = X52_LAYOUT_HEADER_BYTES + 2u;
    uint8_t *buf = (uint8_t *)calloc(1, len);
    assert_non_null(buf);
    memcpy(buf, "X52L", 4);
    write_be16(buf + 4, 99);
    write_be16(buf + 6, 0);
    write_be32(buf + 8, limit);
    memcpy(buf + 16, "v", 2);
    finalize_crc(buf, len);

    x52_layout *L = NULL;
    assert_int_equal(x52_layout_load_memory(buf, len, &L), EINVAL);
    assert_null(L);
    free(buf);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_load_minimal_lookup),
        cmocka_unit_test(test_metadata_plain),
        cmocka_unit_test(test_metadata_truncated_suffix),
        cmocka_unit_test(test_metadata_name_truncated_flag_only),
        cmocka_unit_test(test_reject_unknown_flags),
        cmocka_unit_test(test_reject_empty_name),
        cmocka_unit_test(test_reject_bad_checksum),
        cmocka_unit_test(test_layout_crc_matches_python_zlib_minimal),
        cmocka_unit_test(test_reject_tampered_checksum_byte),
        cmocka_unit_test(test_reject_codepoint_limit_not_big_endian),
        cmocka_unit_test(test_reject_version_word_not_big_endian_one),
        cmocka_unit_test(test_reject_size_mismatch),
        cmocka_unit_test(test_reject_disallowed_usage),
        cmocka_unit_test(test_reject_version),
        cmocka_unit_test(test_basename_normalize_and_join),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
