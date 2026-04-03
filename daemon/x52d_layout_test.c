/*
 * Saitek X52 Pro MFD & LED driver — keyboard layout loader tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>

#define PINELOG_MODULE X52D_MOD_LAYOUT
#include "pinelog.h"
#include "x52d_const.h"
#include "x52d_layout.h"

static void test_sample_layout_lower_a(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=test\n"
        "[a]\n"
        "Key=A\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_non_null(layout);
    assert_string_equal(x52d_layout_get_name(layout), "test");
    assert_true(x52d_layout_lookup(layout, (uint32_t)'a', &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_A);
    assert_int_equal((int)r.mods, (int)VKM_KEY_MOD_NONE);
    x52d_layout_free(layout);
}

static void test_hex_key(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=h\n"
        "[x]\n"
        "Key=0x04\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_true(x52d_layout_lookup(layout, (uint32_t)'x', &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_A);
    x52d_layout_free(layout);
}

static void test_modifiers_combo(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=m\n"
        "[t]\n"
        "Key=T\n"
        "Mods=Ctrl+Shift\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_true(x52d_layout_lookup(layout, (uint32_t)'t', &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_T);
    assert_int_equal((int)r.mods,
                     (int)(VKM_KEY_MOD_LCTRL | VKM_KEY_MOD_LSHIFT));
    x52d_layout_free(layout);
}

static void test_utf8_codepoint(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=u\n"
        "[\xE2\x82\xAC]\n"
        "Key=Enter\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_true(x52d_layout_lookup(layout, 0x20ACU, &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_ENTER);
    x52d_layout_free(layout);
}

static void test_unknown_key_errors(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=bad\n"
        "[q]\n"
        "Key=NotARealKey\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, EINVAL);
    assert_null(layout);
}

static void test_collision_last_section_wins(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=c\n"
        "[z]\n"
        "Key=A\n"
        "Mods=\n"
        "[z]\n"
        "Key=B\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_true(x52d_layout_lookup(layout, (uint32_t)'z', &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_B);
    x52d_layout_free(layout);
}

static void test_unknown_modifier_errors(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=badmod\n"
        "[q]\n"
        "Key=A\n"
        "Mods=HyperMeta\n";
    struct x52d_layout *layout = NULL;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, EINVAL);
    assert_null(layout);
}

static void test_lookup_miss(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=m\n"
        "[a]\n"
        "Key=A\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_false(x52d_layout_lookup(layout, (uint32_t)'b', &r));
    x52d_layout_free(layout);
}

static void test_u_plus_section_escape(void **state)
{
    static const char ini[] =
        "[Layout]\n"
        "Name=e\n"
        "[U+005D]\n"
        "Key=RightBracket\n"
        "Mods=\n";
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_buffer(&layout, ini, strlen(ini));
    assert_int_equal(rc, 0);
    assert_true(x52d_layout_lookup(layout, 0x005DU, &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_RIGHT_BRACKET);
    x52d_layout_free(layout);
}

static void test_us_layout_file(void **state)
{
    struct x52d_layout *layout = NULL;
    struct x52d_layout_recipe r;
    int rc;

    (void)state;
    rc = x52d_layout_load_file(&layout, "layouts.d/us.layout");
    assert_int_equal(rc, 0);
    assert_non_null(layout);
    assert_string_equal(x52d_layout_get_name(layout), "us");
    assert_true(x52d_layout_lookup(layout, (uint32_t)'#', &r));
    assert_int_equal((int)r.key, (int)VKM_KEY_3);
    assert_int_equal((int)r.mods, (int)VKM_KEY_MOD_SHIFT);
    x52d_layout_free(layout);
}

const struct CMUnitTest layout_tests[] = {
    cmocka_unit_test(test_sample_layout_lower_a),
    cmocka_unit_test(test_hex_key),
    cmocka_unit_test(test_modifiers_combo),
    cmocka_unit_test(test_utf8_codepoint),
    cmocka_unit_test(test_unknown_key_errors),
    cmocka_unit_test(test_collision_last_section_wins),
    cmocka_unit_test(test_unknown_modifier_errors),
    cmocka_unit_test(test_lookup_miss),
    cmocka_unit_test(test_u_plus_section_escape),
    cmocka_unit_test(test_us_layout_file),
};

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(layout_tests, NULL, NULL);
    return 0;
}
