/*
 * Saitek virtual keyboard mouse - Linux evdev unit tests
 *
 * Copyright (C) 2026 Nirenjan Krishnan
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <cmocka.h>

#include <libevdev/libevdev-uinput.h>

#include "vkm-internal.h"

/**********************************************************************
 * libevdev stubs (linked instead of libevdev)
 *********************************************************************/
struct libevdev {
    int dummy;
};

struct libevdev_uinput {
    int dummy;
};

static struct libevdev dummy_evdev_device;
static struct libevdev_uinput dummy_uinput_device;

/* When zero, uinput creation "succeeds" and *uidev is set; otherwise that rc is returned. */
static int vkm_test_uinput_create_rc;

struct libevdev *libevdev_new(void)
{
    function_called();
    return mock_ptr_type(struct libevdev *);
}

void libevdev_free(struct libevdev *dev)
{
    check_expected_ptr(dev);
}

int libevdev_uinput_create_from_device(const struct libevdev *dev, int uinput_fd,
                                       struct libevdev_uinput **uidev)
{
    function_called();
    check_expected_ptr(dev);
    check_expected(uinput_fd);
    if (vkm_test_uinput_create_rc != 0) {
        return vkm_test_uinput_create_rc;
    }
    *uidev = &dummy_uinput_device;
    return 0;
}

void libevdev_uinput_destroy(struct libevdev_uinput *uinput_dev)
{
    check_expected_ptr(uinput_dev);
}

int libevdev_enable_event_type(struct libevdev *dev, unsigned int type)
{
    function_called();
    check_expected_ptr(dev);
    check_expected(type);
    return 0;
}

int libevdev_enable_event_code(struct libevdev *dev, unsigned int type, unsigned int code,
                               const void *data)
{
    function_called();
    check_expected_ptr(dev);
    check_expected(type);
    check_expected(code);
    check_expected_ptr(data);
    return 0;
}

void libevdev_set_name(struct libevdev *dev, const char *name)
{
    function_called();
    check_expected_ptr(dev);
    check_expected(name);
}

int libevdev_uinput_write_event(const struct libevdev_uinput *uidev, unsigned int type,
                                unsigned int code, int value)
{
    function_called();
    check_expected_ptr(uidev);
    check_expected(type);
    check_expected(code);
    check_expected(value);
    return mock_type(int);
}

time_t time(time_t *t)
{
    const time_t tv = (time_t)0xdeadbeef;

    if (t) {
        *t = tv;
    }
    return tv;
}

/**********************************************************************
 * Expectation helpers — mirror enable_mouse_events() order
 *********************************************************************/
static void expect_enable_mouse_events(bool hi_res, bool horiz)
{
    expect_function_call(libevdev_enable_event_type);
    expect_value(libevdev_enable_event_type, dev, &dummy_evdev_device);
    expect_value(libevdev_enable_event_type, type, EV_REL);

#define EXPECT_REL_CODE(evcode)                               \
    do {                                                       \
        expect_function_call(libevdev_enable_event_code);      \
        expect_value(libevdev_enable_event_code, dev, &dummy_evdev_device); \
        expect_value(libevdev_enable_event_code, type, EV_REL);           \
        expect_value(libevdev_enable_event_code, code, (evcode));        \
        expect_value(libevdev_enable_event_code, data, (void *)NULL);   \
    } while (0)

    EXPECT_REL_CODE(REL_X);
    EXPECT_REL_CODE(REL_Y);
    EXPECT_REL_CODE(REL_WHEEL);
    if (hi_res) {
        EXPECT_REL_CODE(REL_WHEEL_HI_RES);
    }
    if (horiz) {
        EXPECT_REL_CODE(REL_HWHEEL);
        if (hi_res) {
            EXPECT_REL_CODE(REL_HWHEEL_HI_RES);
        }
    }
#undef EXPECT_REL_CODE

    expect_function_call(libevdev_enable_event_type);
    expect_value(libevdev_enable_event_type, dev, &dummy_evdev_device);
    expect_value(libevdev_enable_event_type, type, EV_KEY);

#define EXPECT_KEY_CODE(keycode)                              \
    do {                                                       \
        expect_function_call(libevdev_enable_event_code);      \
        expect_value(libevdev_enable_event_code, dev, &dummy_evdev_device); \
        expect_value(libevdev_enable_event_code, type, EV_KEY);           \
        expect_value(libevdev_enable_event_code, code, (keycode));        \
        expect_value(libevdev_enable_event_code, data, (void *)NULL);     \
    } while (0)

    EXPECT_KEY_CODE(BTN_LEFT);
    EXPECT_KEY_CODE(BTN_RIGHT);
    EXPECT_KEY_CODE(BTN_MIDDLE);
#undef EXPECT_KEY_CODE
}

/** Same order as \c enable_keyboard_events() in \c linux_evdev.c (skip unmapped indices). */
static const unsigned int expect_kb_codes[] = {
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
    KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_ENTER, KEY_ESC, KEY_BACKSPACE, KEY_TAB, KEY_SPACE, KEY_MINUS, KEY_EQUAL, KEY_LEFTBRACE,
    KEY_RIGHTBRACE, KEY_BACKSLASH, KEY_NUMERIC_POUND, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_GRAVE,
    KEY_COMMA, KEY_DOT, KEY_SLASH,
    KEY_CAPSLOCK,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    KEY_PRINT, KEY_SCROLLLOCK, KEY_PAUSE, KEY_INSERT, KEY_HOME, KEY_PAGEUP, KEY_DELETE, KEY_END,
    KEY_PAGEDOWN, KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP,
    KEY_NUMLOCK, KEY_KPSLASH, KEY_KPASTERISK, KEY_KPMINUS, KEY_KPPLUS, KEY_KPENTER,
    KEY_KP1, KEY_KP2, KEY_KP3, KEY_KP4, KEY_KP5, KEY_KP6, KEY_KP7, KEY_KP8, KEY_KP9, KEY_KP0,
    KEY_KPDOT, KEY_102ND, KEY_MENU, KEY_KPCOMMA,
    KEY_LEFTCTRL, KEY_LEFTSHIFT, KEY_LEFTALT, KEY_LEFTMETA, KEY_RIGHTCTRL, KEY_RIGHTSHIFT,
    KEY_RIGHTALT, KEY_RIGHTMETA,
};

static void expect_enable_keyboard_event_codes(void)
{
    size_t i;

    for (i = 0; i < sizeof(expect_kb_codes) / sizeof(expect_kb_codes[0]); i++) {
        expect_function_call(libevdev_enable_event_code);
        expect_value(libevdev_enable_event_code, dev, &dummy_evdev_device);
        expect_value(libevdev_enable_event_code, type, EV_KEY);
        expect_value(libevdev_enable_event_code, code, expect_kb_codes[i]);
        expect_value(libevdev_enable_event_code, data, (void *)NULL);
    }
}

static void expect_uinput_create_ok(void)
{
    vkm_test_uinput_create_rc = 0;
    expect_function_call(libevdev_uinput_create_from_device);
    expect_value(libevdev_uinput_create_from_device, dev, &dummy_evdev_device);
    expect_value(libevdev_uinput_create_from_device, uinput_fd, LIBEVDEV_UINPUT_OPEN_MANAGED);
}

/* First successful vkm_start: create dev, name, enable events, uinput. */
static void expect_first_start_sequence(const char *device_name, bool hi_res, bool horiz)
{
    expect_function_call(libevdev_new);
    will_return(libevdev_new, &dummy_evdev_device);

    expect_function_call(libevdev_set_name);
    expect_value(libevdev_set_name, dev, &dummy_evdev_device);
    expect_string(libevdev_set_name, name, device_name);

    expect_enable_mouse_events(hi_res, horiz);
    expect_enable_keyboard_event_codes();
    expect_uinput_create_ok();
}

static void expect_uinput_sync_report_ok(void)
{
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_SYN);
    expect_value(libevdev_uinput_write_event, code, SYN_REPORT);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
}

static int teardown_ctx(void **state)
{
    vkm_context *ctx = *state;

    vkm_test_uinput_create_rc = 0;
    if (ctx != NULL) {
        if (vkm_is_ready(ctx)) {
            expect_uinput_sync_report_ok();
            expect_value(libevdev_uinput_destroy, uinput_dev, &dummy_uinput_device);
            expect_value(libevdev_free, dev, &dummy_evdev_device);
        }
        vkm_exit(ctx);
    }
    *state = NULL;
    return 0;
}

/**********************************************************************
 * Tests: init / exit / queries
 *********************************************************************/
static void test_init_null_out(void **state)
{
    (void)state;
    assert_int_equal(vkm_init(NULL), VKM_ERROR_INVALID_PARAM);
}

static void test_exit_null(void **state)
{
    (void)state;
    vkm_exit(NULL);
}

static void test_platform_supported(void **state)
{
    (void)state;
    assert_true(vkm_platform_supported());
}

static void test_feature_supported(void **state)
{
    (void)state;
    assert_true(vkm_feature_supported(VKM_FEAT_MOUSE));
    assert_true(vkm_feature_supported(VKM_FEAT_KEYBOARD));
    assert_true(vkm_feature_supported(VKM_FEAT_KEYBOARD_MODIFIERS));
    assert_false(vkm_feature_supported((vkm_feature)99));
}

static void test_is_ready_null(void **state)
{
    (void)state;
    assert_false(vkm_is_ready(NULL));
}

static void test_is_ready_after_init_only(void **state)
{
    vkm_context *ctx = NULL;

    (void)state;
    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_non_null(ctx);
    assert_false(vkm_is_ready(ctx));
    vkm_exit(ctx);
}

/**********************************************************************
 * Tests: vkm_set_option
 *********************************************************************/
static void test_set_option_null_ctx(void **state)
{
    (void)state;
    assert_int_equal(vkm_set_option(NULL, VKM_OPT_HI_RES_SCROLL, 1), VKM_ERROR_INVALID_PARAM);
}

static void test_set_option_unknown(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_set_option(ctx, (vkm_option)999, 0), VKM_ERROR_INVALID_PARAM);
}

static void test_set_option_flags_and_name(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;

    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HI_RES_SCROLL, 1), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HORIZONTAL_SCROLL, 1), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_DEVICE_NAME, "custom-name"), VKM_SUCCESS);

    expect_first_start_sequence("custom-name", true, true);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);
}

/**********************************************************************
 * Tests: vkm_start
 *********************************************************************/
static void test_start_null(void **state)
{
    (void)state;
    assert_int_equal(vkm_start(NULL), VKM_ERROR_INVALID_PARAM);
}

static void test_start_libevdev_new_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;

    expect_function_call(libevdev_new);
    will_return(libevdev_new, (struct libevdev *)(NULL));

    assert_int_equal(vkm_start(ctx), VKM_ERROR_DEV_FAILURE);
}

static void test_start_uinput_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;

    vkm_test_uinput_create_rc = -1;

    expect_function_call(libevdev_new);
    will_return(libevdev_new, &dummy_evdev_device);

    expect_function_call(libevdev_set_name);
    expect_value(libevdev_set_name, dev, &dummy_evdev_device);
    expect_string(libevdev_set_name, name, "VKM virtual device @deadbeef");

    expect_enable_mouse_events(false, false);
    expect_enable_keyboard_event_codes();

    expect_function_call(libevdev_uinput_create_from_device);
    expect_value(libevdev_uinput_create_from_device, dev, &dummy_evdev_device);
    expect_value(libevdev_uinput_create_from_device, uinput_fd, LIBEVDEV_UINPUT_OPEN_MANAGED);

    expect_value(libevdev_free, dev, &dummy_evdev_device);

    assert_int_equal(vkm_start(ctx), VKM_ERROR_DEV_FAILURE);
}

static void test_start_default_name_success(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;

    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);
    assert_true(vkm_is_ready(ctx));
}

static void test_start_hi_res_scroll_only(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HI_RES_SCROLL, 1), VKM_SUCCESS);
    *state = ctx;

    expect_first_start_sequence("VKM virtual device @deadbeef", true, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);
}

static void test_start_horizontal_scroll_only(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HORIZONTAL_SCROLL, 1), VKM_SUCCESS);
    *state = ctx;

    expect_first_start_sequence("VKM virtual device @deadbeef", false, true);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);
}

static void test_start_twice_idempotent(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;

    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);
}

/**********************************************************************
 * Tests: mouse move
 *********************************************************************/
static void test_mouse_move_param_and_ready(void **state)
{
    assert_int_equal(vkm_mouse_move(NULL, 1, 1), VKM_ERROR_INVALID_PARAM);

    vkm_context *ctx = NULL;
    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_mouse_move(ctx, 1, 1), VKM_ERROR_NOT_READY);
}

static void test_mouse_move_no_change(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    assert_int_equal(vkm_mouse_move(ctx, 0, 0), VKM_ERROR_NO_CHANGE);
}

static void test_mouse_move_dx_then_dy(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_X);
    expect_value(libevdev_uinput_write_event, value, 3);
    will_return(libevdev_uinput_write_event, 0);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_Y);
    expect_value(libevdev_uinput_write_event, value, -2);
    will_return(libevdev_uinput_write_event, 0);

    assert_int_equal(vkm_mouse_move(ctx, 3, -2), VKM_SUCCESS);
}

static void test_mouse_move_dx_only(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_X);
    expect_value(libevdev_uinput_write_event, value, -9);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_move(ctx, -9, 0), VKM_SUCCESS);
}

static void test_mouse_move_write_dx_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_X);
    expect_value(libevdev_uinput_write_event, value, 5);
    will_return(libevdev_uinput_write_event, -1);

    assert_int_equal(vkm_mouse_move(ctx, 5, 0), VKM_ERROR_EVENT);
}

static void test_mouse_move_write_dy_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_Y);
    expect_value(libevdev_uinput_write_event, value, 7);
    will_return(libevdev_uinput_write_event, -1);

    assert_int_equal(vkm_mouse_move(ctx, 0, 7), VKM_ERROR_EVENT);
}

/**********************************************************************
 * Tests: mouse click
 *********************************************************************/
static void test_mouse_click_invalid(void **state)
{
    vkm_context *ctx = NULL;
    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    assert_int_equal(vkm_mouse_click(NULL, VKM_MOUSE_BTN_LEFT, VKM_BUTTON_PRESSED),
                     VKM_ERROR_INVALID_PARAM);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_MAX, VKM_BUTTON_PRESSED),
                     VKM_ERROR_INVALID_PARAM);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_LEFT, VKM_BUTTON_STATE_MAX),
                     VKM_ERROR_INVALID_PARAM);
}

static void test_mouse_click_not_ready(void **state)
{
    vkm_context *ctx = NULL;
    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_LEFT, VKM_BUTTON_PRESSED),
                     VKM_ERROR_NOT_READY);
}

static void test_mouse_click_no_change(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_LEFT);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_LEFT, VKM_BUTTON_PRESSED), VKM_SUCCESS);

    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_LEFT, VKM_BUTTON_PRESSED),
                     VKM_ERROR_NO_CHANGE);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_LEFT);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    expect_uinput_sync_report_ok();
    assert_int_equal(vkm_reset(ctx), VKM_SUCCESS);
}

static void test_mouse_click_success_release(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_RIGHT);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_RIGHT, VKM_BUTTON_PRESSED), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_RIGHT);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_RIGHT, VKM_BUTTON_RELEASED), VKM_SUCCESS);
}

static void test_mouse_click_write_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_MIDDLE);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, -1);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_MIDDLE, VKM_BUTTON_PRESSED),
                     VKM_ERROR_EVENT);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_MIDDLE);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    expect_uinput_sync_report_ok();
    assert_int_equal(vkm_reset(ctx), VKM_SUCCESS);
}

/**********************************************************************
 * Tests: scroll
 *********************************************************************/
static void test_mouse_scroll_param_ready(void **state)
{
    assert_int_equal(vkm_mouse_scroll(NULL, VKM_MOUSE_SCROLL_UP), VKM_ERROR_INVALID_PARAM);

    vkm_context *ctx = NULL;
    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_MAX), VKM_ERROR_INVALID_PARAM);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_UP), VKM_ERROR_NOT_READY);
}

static void test_mouse_scroll_horizontal_disabled(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_LEFT), VKM_ERROR_NOT_SUPPORTED);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_RIGHT), VKM_ERROR_NOT_SUPPORTED);
}

static void test_mouse_scroll_vertical(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_UP), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL);
    expect_value(libevdev_uinput_write_event, value, -1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_DOWN), VKM_SUCCESS);
}

static void test_mouse_scroll_vertical_hi_res(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HI_RES_SCROLL, 1), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", true, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL_HI_RES);
    expect_value(libevdev_uinput_write_event, value, 120);
    will_return(libevdev_uinput_write_event, 0);
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_UP), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL_HI_RES);
    expect_value(libevdev_uinput_write_event, value, -120);
    will_return(libevdev_uinput_write_event, 0);
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL);
    expect_value(libevdev_uinput_write_event, value, -1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_DOWN), VKM_SUCCESS);
}

static void test_mouse_scroll_horizontal_enabled(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HORIZONTAL_SCROLL, 1), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, true);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_HWHEEL);
    expect_value(libevdev_uinput_write_event, value, -1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_LEFT), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_HWHEEL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_RIGHT), VKM_SUCCESS);
}

static void test_mouse_scroll_horizontal_hi_res(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HI_RES_SCROLL, 1), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HORIZONTAL_SCROLL, 1), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", true, true);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_HWHEEL_HI_RES);
    expect_value(libevdev_uinput_write_event, value, -120);
    will_return(libevdev_uinput_write_event, 0);
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_HWHEEL);
    expect_value(libevdev_uinput_write_event, value, -1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_LEFT), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_HWHEEL_HI_RES);
    expect_value(libevdev_uinput_write_event, value, 120);
    will_return(libevdev_uinput_write_event, 0);
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_HWHEEL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_RIGHT), VKM_SUCCESS);
}

static void test_mouse_scroll_write_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, -1);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_UP), VKM_ERROR_EVENT);
}

static void test_mouse_scroll_write_fails_hi_res(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    assert_int_equal(vkm_set_option(ctx, VKM_OPT_HI_RES_SCROLL, 1), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", true, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_REL);
    expect_value(libevdev_uinput_write_event, code, REL_WHEEL_HI_RES);
    expect_value(libevdev_uinput_write_event, value, 120);
    will_return(libevdev_uinput_write_event, -1);
    assert_int_equal(vkm_mouse_scroll(ctx, VKM_MOUSE_SCROLL_UP), VKM_ERROR_EVENT);
}

/**********************************************************************
 * Tests: vkm_keyboard_send
 *********************************************************************/
static void test_keyboard_send_invalid(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_keyboard_send(NULL, VKM_KEY_A, VKM_KEY_MOD_NONE, VKM_KEY_STATE_PRESSED),
                     VKM_ERROR_INVALID_PARAM);

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    assert_int_equal(vkm_keyboard_send(ctx, VKM_KEY_MAX, VKM_KEY_MOD_NONE, VKM_KEY_STATE_PRESSED),
                     VKM_ERROR_INVALID_PARAM);
    assert_int_equal(vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_NONE, VKM_KEY_STATE_MAX),
                     VKM_ERROR_INVALID_PARAM);
}

static void test_keyboard_send_not_ready(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_NONE, VKM_KEY_STATE_PRESSED),
                     VKM_ERROR_NOT_READY);
}

static void test_keyboard_send_no_modifiers(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_A);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_NONE, VKM_KEY_STATE_PRESSED),
                     VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_A);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_NONE, VKM_KEY_STATE_RELEASED),
                     VKM_SUCCESS);
}

static void test_keyboard_send_with_modifiers(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_LEFTCTRL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_A);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(
            vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_LCTRL, VKM_KEY_STATE_PRESSED), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_A);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_LEFTCTRL);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(
            vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_NONE, VKM_KEY_STATE_RELEASED), VKM_SUCCESS);
}

/**********************************************************************
 * Tests: vkm_reset / vkm_exit cleanup
 *********************************************************************/
static void test_reset_null(void **state)
{
    (void)state;
    assert_int_equal(vkm_reset(NULL), VKM_ERROR_INVALID_PARAM);
}

static void test_reset_not_ready(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_reset(ctx), VKM_SUCCESS);
}

static void test_reset_releases_stuck_keys(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_LEFTCTRL);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_A);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(
            vkm_keyboard_send(ctx, VKM_KEY_A, VKM_KEY_MOD_LCTRL, VKM_KEY_STATE_PRESSED), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_A);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_LEFTCTRL);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    expect_uinput_sync_report_ok();
    assert_int_equal(vkm_reset(ctx), VKM_SUCCESS);
}

static void test_reset_releases_mouse_buttons(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_LEFT);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_mouse_click(ctx, VKM_MOUSE_BTN_LEFT, VKM_BUTTON_PRESSED), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, BTN_LEFT);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    expect_uinput_sync_report_ok();
    assert_int_equal(vkm_reset(ctx), VKM_SUCCESS);
}

static void test_exit_calls_reset_on_stuck_key(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_B);
    expect_value(libevdev_uinput_write_event, value, 1);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_keyboard_send(ctx, VKM_KEY_B, VKM_KEY_MOD_NONE, VKM_KEY_STATE_PRESSED),
                     VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_KEY);
    expect_value(libevdev_uinput_write_event, code, KEY_B);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);

    expect_uinput_sync_report_ok();

    expect_value(libevdev_uinput_destroy, uinput_dev, &dummy_uinput_device);
    expect_value(libevdev_free, dev, &dummy_evdev_device);

    vkm_exit(ctx);
    *state = NULL;
}

/**********************************************************************
 * Tests: sync
 *********************************************************************/
static void test_sync_null(void **state)
{
    (void)state;
    assert_int_equal(vkm_sync(NULL), VKM_ERROR_INVALID_PARAM);
}

static void test_sync_not_ready(void **state)
{
    vkm_context *ctx = NULL;
    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    assert_int_equal(vkm_sync(ctx), VKM_ERROR_NOT_READY);
}

static void test_sync_success(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_SYN);
    expect_value(libevdev_uinput_write_event, code, SYN_REPORT);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, 0);
    assert_int_equal(vkm_sync(ctx), VKM_SUCCESS);
}

static void test_sync_write_fails(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_function_call(libevdev_uinput_write_event);
    expect_value(libevdev_uinput_write_event, uidev, &dummy_uinput_device);
    expect_value(libevdev_uinput_write_event, type, EV_SYN);
    expect_value(libevdev_uinput_write_event, code, SYN_REPORT);
    expect_value(libevdev_uinput_write_event, value, 0);
    will_return(libevdev_uinput_write_event, -1);
    assert_int_equal(vkm_sync(ctx), VKM_ERROR_EVENT);
}

/**********************************************************************
 * Tests: vkm_exit destroys uinput + dev
 *********************************************************************/
static void test_exit_after_start(void **state)
{
    vkm_context *ctx = NULL;

    assert_int_equal(vkm_init(&ctx), VKM_SUCCESS);
    *state = ctx;
    expect_first_start_sequence("VKM virtual device @deadbeef", false, false);
    assert_int_equal(vkm_start(ctx), VKM_SUCCESS);

    expect_uinput_sync_report_ok();

    expect_value(libevdev_uinput_destroy, uinput_dev, &dummy_uinput_device);
    expect_value(libevdev_free, dev, &dummy_evdev_device);

    vkm_exit(ctx);
    *state = NULL;
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_null_out),
        cmocka_unit_test(test_exit_null),
        cmocka_unit_test(test_platform_supported),
        cmocka_unit_test(test_feature_supported),
        cmocka_unit_test(test_is_ready_null),
        cmocka_unit_test(test_is_ready_after_init_only),
        cmocka_unit_test(test_set_option_null_ctx),
        cmocka_unit_test_setup_teardown(test_set_option_unknown, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_set_option_flags_and_name, NULL, teardown_ctx),
        cmocka_unit_test(test_start_null),
        cmocka_unit_test_setup_teardown(test_start_libevdev_new_fails, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_start_uinput_fails, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_start_default_name_success, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_start_hi_res_scroll_only, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_start_horizontal_scroll_only, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_start_twice_idempotent, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_move_param_and_ready, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_move_no_change, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_move_dx_then_dy, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_move_dx_only, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_move_write_dx_fails, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_move_write_dy_fails, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_click_invalid, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_click_not_ready, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_click_no_change, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_click_success_release, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_click_write_fails, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_param_ready, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_horizontal_disabled, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_vertical, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_vertical_hi_res, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_horizontal_enabled, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_horizontal_hi_res, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_write_fails, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_mouse_scroll_write_fails_hi_res, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_keyboard_send_invalid, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_keyboard_send_not_ready, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_keyboard_send_no_modifiers, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_keyboard_send_with_modifiers, NULL, teardown_ctx),
        cmocka_unit_test(test_reset_null),
        cmocka_unit_test_setup_teardown(test_reset_not_ready, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_reset_releases_stuck_keys, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_reset_releases_mouse_buttons, NULL, teardown_ctx),
        cmocka_unit_test(test_exit_calls_reset_on_stuck_key),
        cmocka_unit_test(test_sync_null),
        cmocka_unit_test_setup_teardown(test_sync_not_ready, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_sync_success, NULL, teardown_ctx),
        cmocka_unit_test_setup_teardown(test_sync_write_fails, NULL, teardown_ctx),
        cmocka_unit_test(test_exit_after_start),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
