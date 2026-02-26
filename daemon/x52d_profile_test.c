/*
 * Saitek X52 Pro MFD & LED driver - Profile module tests
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <unistd.h>
#include <cmocka.h>

#include "libevdev/libevdev.h"
#include "libx52io.h"

#define PINELOG_MODULE X52D_MOD_PROFILE
#include "pinelog.h"

#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_keyboard.h"
#include "x52d_profile.h"

/* Stub keyboard: record (key_code, value) for assertions */
#define MAX_RECORDED 32
static struct { uint16_t code; int value; } key_record[MAX_RECORDED];
static size_t key_record_n;

int x52d_keyboard_evdev_key(uint16_t key_code, int value)
{
    if (key_record_n < MAX_RECORDED) {
        key_record[key_record_n].code = key_code;
        key_record[key_record_n].value = value;
        key_record_n++;
    }
    return 0;
}

bool x52d_keyboard_evdev_available(void)
{
    return true;
}

static void key_record_reset(void)
{
    key_record_n = 0;
}

static int key_code_from_name(const char *name)
{
    int c = libevdev_event_code_from_name(EV_KEY, name);
    return (c >= 0) ? c : -1;
}

static void write_profile_file(const char *path, const char *content)
{
    FILE *f = fopen(path, "w");
    assert_non_null(f);
    assert_true(fputs(content, f) >= 0);
    fclose(f);
}

static void test_profile_single_key(void **state)
{
    char tmpdir[] = "/tmp/x52d_profile_test_XXXXXX";
    char path[256];
    libx52io_report report, prev;

    (void)state;
    assert_non_null(mkdtemp(tmpdir));

    snprintf(path, sizeof(path), "%s/test.conf", tmpdir);
    write_profile_file(path,
        "[Mode1]\n"
        "Button.BTN_FIRE = key KEY_E\n");

    x52d_config_set("Profiles", "Directory", tmpdir);
    x52d_config_set("Profiles", "Profile", "test");
    x52d_profile_init();

    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.mode = 1;
    report.button[LIBX52IO_BTN_FIRE] = true;

    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 1);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_E"));
    assert_int_equal(key_record[0].value, 1);

    memcpy(&prev, &report, sizeof(report));
    report.button[LIBX52IO_BTN_FIRE] = false;
    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 1);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_E"));
    assert_int_equal(key_record[0].value, 0);

    x52d_profile_exit();
    unlink(path);
    rmdir(tmpdir);
}

static void test_profile_key_combo(void **state)
{
    char tmpdir[] = "/tmp/x52d_profile_test_XXXXXX";
    char path[256];
    libx52io_report report, prev;

    (void)state;
    assert_non_null(mkdtemp(tmpdir));

    snprintf(path, sizeof(path), "%s/test.conf", tmpdir);
    write_profile_file(path,
        "[Mode1]\n"
        "Button.BTN_A = key KEY_LEFTCTRL KEY_X\n");

    x52d_config_set("Profiles", "Directory", tmpdir);
    x52d_config_set("Profiles", "Profile", "test");
    x52d_profile_init();

    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.mode = 1;
    report.button[LIBX52IO_BTN_A] = true;

    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 2);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_LEFTCTRL"));
    assert_int_equal(key_record[0].value, 1);
    assert_int_equal(key_record[1].code, key_code_from_name("KEY_X"));
    assert_int_equal(key_record[1].value, 1);

    memcpy(&prev, &report, sizeof(report));
    report.button[LIBX52IO_BTN_A] = false;
    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 2);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_X"));
    assert_int_equal(key_record[0].value, 0);
    assert_int_equal(key_record[1].code, key_code_from_name("KEY_LEFTCTRL"));
    assert_int_equal(key_record[1].value, 0);

    x52d_profile_exit();
    unlink(path);
    rmdir(tmpdir);
}

static void test_profile_macro_button_down_only(void **state)
{
    char tmpdir[] = "/tmp/x52d_profile_test_XXXXXX";
    char path[256];
    libx52io_report report, prev;

    (void)state;
    assert_non_null(mkdtemp(tmpdir));

    snprintf(path, sizeof(path), "%s/test.conf", tmpdir);
    write_profile_file(path,
        "[Mode1]\n"
        "Button.BTN_T1_UP = macro KEY_A KEY_B\n");

    x52d_config_set("Profiles", "Directory", tmpdir);
    x52d_config_set("Profiles", "Profile", "test");
    x52d_profile_init();

    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.mode = 1;
    report.button[LIBX52IO_BTN_T1_UP] = true;

    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 4);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_A"));
    assert_int_equal(key_record[0].value, 1);
    assert_int_equal(key_record[1].code, key_code_from_name("KEY_A"));
    assert_int_equal(key_record[1].value, 0);
    assert_int_equal(key_record[2].code, key_code_from_name("KEY_B"));
    assert_int_equal(key_record[2].value, 1);
    assert_int_equal(key_record[3].code, key_code_from_name("KEY_B"));
    assert_int_equal(key_record[3].value, 0);

    memcpy(&prev, &report, sizeof(report));
    report.button[LIBX52IO_BTN_T1_UP] = false;
    key_record_reset();
    x52d_profile_apply(&report, &prev);
    assert_int_equal(key_record_n, 0);

    x52d_profile_exit();
    unlink(path);
    rmdir(tmpdir);
}

static void test_profile_fallback_mode2_to_mode1(void **state)
{
    char tmpdir[] = "/tmp/x52d_profile_test_XXXXXX";
    char path[256];
    libx52io_report report, prev;

    (void)state;
    assert_non_null(mkdtemp(tmpdir));

    snprintf(path, sizeof(path), "%s/test.conf", tmpdir);
    write_profile_file(path,
        "[Mode1]\n"
        "Button.BTN_FIRE = key KEY_E\n");

    x52d_config_set("Profiles", "Directory", tmpdir);
    x52d_config_set("Profiles", "Profile", "test");
    x52d_profile_init();

    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.mode = 2;
    report.button[LIBX52IO_BTN_FIRE] = true;

    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 1);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_E"));
    assert_int_equal(key_record[0].value, 1);

    x52d_profile_exit();
    unlink(path);
    rmdir(tmpdir);
}

static void test_profile_shift_layer(void **state)
{
    char tmpdir[] = "/tmp/x52d_profile_test_XXXXXX";
    char path[256];
    libx52io_report report, prev;

    (void)state;
    assert_non_null(mkdtemp(tmpdir));

    snprintf(path, sizeof(path), "%s/test.conf", tmpdir);
    write_profile_file(path,
        "[Profile]\n"
        "ShiftButton=BTN_PINKY\n"
        "[Mode1]\n"
        "Button.BTN_FIRE = key KEY_E\n"
        "[Mode1.Shift]\n"
        "Button.BTN_FIRE = key KEY_F\n");

    x52d_config_set("Profiles", "Directory", tmpdir);
    x52d_config_set("Profiles", "Profile", "test");
    x52d_profile_init();

    memset(&report, 0, sizeof(report));
    memset(&prev, 0, sizeof(prev));
    report.mode = 1;
    report.button[LIBX52IO_BTN_PINKY] = true;
    report.button[LIBX52IO_BTN_FIRE] = true;

    key_record_reset();
    x52d_profile_apply(&report, &prev);

    assert_int_equal(key_record_n, 1);
    assert_int_equal(key_record[0].code, key_code_from_name("KEY_F"));
    assert_int_equal(key_record[0].value, 1);

    x52d_profile_exit();
    unlink(path);
    rmdir(tmpdir);
}

static void test_profile_get_name(void **state)
{
    char tmpdir[] = "/tmp/x52d_profile_test_XXXXXX";
    char path[256];
    const char *name;

    (void)state;
    assert_non_null(mkdtemp(tmpdir));

    snprintf(path, sizeof(path), "%s/test.conf", tmpdir);
    write_profile_file(path,
        "[Profile]\n"
        "Name=My Test Profile\n"
        "[Mode1]\n"
        "Button.BTN_FIRE = key KEY_E\n");

    x52d_config_set("Profiles", "Directory", tmpdir);
    x52d_config_set("Profiles", "Profile", "test");
    x52d_profile_init();

    name = x52d_profile_get_name();
    assert_non_null(name);
    assert_string_equal(name, "My Test Profile");

    x52d_profile_exit();
    name = x52d_profile_get_name();
    assert_null(name);
    unlink(path);
    rmdir(tmpdir);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_profile_single_key),
        cmocka_unit_test(test_profile_key_combo),
        cmocka_unit_test(test_profile_macro_button_down_only),
        cmocka_unit_test(test_profile_fallback_mode2_to_mode1),
        cmocka_unit_test(test_profile_shift_layer),
        cmocka_unit_test(test_profile_get_name),
    };
    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
