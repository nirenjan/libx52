/*
 * VKM Linux evdev implementation
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 *
 * SPDX-LicenseIdentifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#include "libevdev/libevdev.h"
#include "libevdev/libevdev-uinput.h"

#include "vkm-internal.h"

/** evdev codes; index by \ref vkm_key (HID usage). \c -1 if unmapped. */
static const int vkm_key_to_evdev[VKM_KEY_MAX] = {
#if defined(__GNUC__) || defined(__clang__)
    [0 ...(VKM_KEY_MAX - 1)] = -1,
#else
#error "vkm_linux_evdev.c requires GCC or Clang (sparse vkm_key_to_evdev initializer)"
#endif

    [VKM_KEY_NONE] = -1,

    [VKM_KEY_LEFT_CTRL] = KEY_LEFTCTRL,
    [VKM_KEY_LEFT_ALT] = KEY_LEFTALT,
    [VKM_KEY_LEFT_GUI] = KEY_LEFTMETA,
    [VKM_KEY_RIGHT_CTRL] = KEY_RIGHTCTRL,
    [VKM_KEY_RIGHT_ALT] = KEY_RIGHTALT,
    [VKM_KEY_RIGHT_GUI] = KEY_RIGHTMETA,
    [VKM_KEY_APPLICATION] = KEY_MENU,

    [VKM_KEY_ESCAPE] = KEY_ESC,
    [VKM_KEY_F1] = KEY_F1,
    [VKM_KEY_F2] = KEY_F2,
    [VKM_KEY_F3] = KEY_F3,
    [VKM_KEY_F4] = KEY_F4,
    [VKM_KEY_F5] = KEY_F5,
    [VKM_KEY_F6] = KEY_F6,
    [VKM_KEY_F7] = KEY_F7,
    [VKM_KEY_F8] = KEY_F8,
    [VKM_KEY_F9] = KEY_F9,
    [VKM_KEY_F10] = KEY_F10,
    [VKM_KEY_F11] = KEY_F11,
    [VKM_KEY_F12] = KEY_F12,

    [VKM_KEY_GRAVE_ACCENT] = KEY_GRAVE,
    [VKM_KEY_1] = KEY_1,
    [VKM_KEY_2] = KEY_2,
    [VKM_KEY_3] = KEY_3,
    [VKM_KEY_4] = KEY_4,
    [VKM_KEY_5] = KEY_5,
    [VKM_KEY_6] = KEY_6,
    [VKM_KEY_7] = KEY_7,
    [VKM_KEY_8] = KEY_8,
    [VKM_KEY_9] = KEY_9,
    [VKM_KEY_0] = KEY_0,
    [VKM_KEY_MINUS] = KEY_MINUS,
    [VKM_KEY_EQUAL] = KEY_EQUAL,
    [VKM_KEY_BACKSPACE] = KEY_BACKSPACE,

    [VKM_KEY_TAB] = KEY_TAB,
    [VKM_KEY_Q] = KEY_Q,
    [VKM_KEY_W] = KEY_W,
    [VKM_KEY_E] = KEY_E,
    [VKM_KEY_R] = KEY_R,
    [VKM_KEY_T] = KEY_T,
    [VKM_KEY_Y] = KEY_Y,
    [VKM_KEY_U] = KEY_U,
    [VKM_KEY_I] = KEY_I,
    [VKM_KEY_O] = KEY_O,
    [VKM_KEY_P] = KEY_P,
    [VKM_KEY_LEFT_BRACKET] = KEY_LEFTBRACE,
    [VKM_KEY_RIGHT_BRACKET] = KEY_RIGHTBRACE,
    [VKM_KEY_BACKSLASH] = KEY_BACKSLASH,

    [VKM_KEY_CAPS_LOCK] = KEY_CAPSLOCK,
    [VKM_KEY_A] = KEY_A,
    [VKM_KEY_S] = KEY_S,
    [VKM_KEY_D] = KEY_D,
    [VKM_KEY_F] = KEY_F,
    [VKM_KEY_G] = KEY_G,
    [VKM_KEY_H] = KEY_H,
    [VKM_KEY_J] = KEY_J,
    [VKM_KEY_K] = KEY_K,
    [VKM_KEY_L] = KEY_L,
    [VKM_KEY_SEMICOLON] = KEY_SEMICOLON,
    [VKM_KEY_APOSTROPHE] = KEY_APOSTROPHE,
    [VKM_KEY_NONUS_HASH] = KEY_NUMERIC_POUND,
    [VKM_KEY_ENTER] = KEY_ENTER,

    [VKM_KEY_LEFT_SHIFT] = KEY_LEFTSHIFT,
    [VKM_KEY_INTL_BACKSLASH] = KEY_102ND,
    [VKM_KEY_Z] = KEY_Z,
    [VKM_KEY_X] = KEY_X,
    [VKM_KEY_C] = KEY_C,
    [VKM_KEY_V] = KEY_V,
    [VKM_KEY_B] = KEY_B,
    [VKM_KEY_N] = KEY_N,
    [VKM_KEY_M] = KEY_M,
    [VKM_KEY_COMMA] = KEY_COMMA,
    [VKM_KEY_PERIOD] = KEY_DOT,
    [VKM_KEY_SLASH] = KEY_SLASH,
    [VKM_KEY_RIGHT_SHIFT] = KEY_RIGHTSHIFT,

    [VKM_KEY_SPACE] = KEY_SPACE,

    [VKM_KEY_PRINT_SCREEN] = KEY_PRINT,
    [VKM_KEY_SCROLL_LOCK] = KEY_SCROLLLOCK,
    [VKM_KEY_PAUSE] = KEY_PAUSE,

    [VKM_KEY_INSERT] = KEY_INSERT,
    [VKM_KEY_HOME] = KEY_HOME,
    [VKM_KEY_PAGE_UP] = KEY_PAGEUP,
    [VKM_KEY_DELETE_FORWARD] = KEY_DELETE,
    [VKM_KEY_END] = KEY_END,
    [VKM_KEY_PAGE_DOWN] = KEY_PAGEDOWN,

    [VKM_KEY_RIGHT_ARROW] = KEY_RIGHT,
    [VKM_KEY_LEFT_ARROW] = KEY_LEFT,
    [VKM_KEY_DOWN_ARROW] = KEY_DOWN,
    [VKM_KEY_UP_ARROW] = KEY_UP,

    [VKM_KEY_KEYPAD_NUM_LOCK] = KEY_NUMLOCK,
    [VKM_KEY_KEYPAD_DIVIDE] = KEY_KPSLASH,
    [VKM_KEY_KEYPAD_MULTIPLY] = KEY_KPASTERISK,
    [VKM_KEY_KEYPAD_MINUS] = KEY_KPMINUS,
    [VKM_KEY_KEYPAD_PLUS] = KEY_KPPLUS,
    [VKM_KEY_KEYPAD_ENTER] = KEY_KPENTER,
    [VKM_KEY_KEYPAD_1] = KEY_KP1,
    [VKM_KEY_KEYPAD_2] = KEY_KP2,
    [VKM_KEY_KEYPAD_3] = KEY_KP3,
    [VKM_KEY_KEYPAD_4] = KEY_KP4,
    [VKM_KEY_KEYPAD_5] = KEY_KP5,
    [VKM_KEY_KEYPAD_6] = KEY_KP6,
    [VKM_KEY_KEYPAD_7] = KEY_KP7,
    [VKM_KEY_KEYPAD_8] = KEY_KP8,
    [VKM_KEY_KEYPAD_9] = KEY_KP9,
    [VKM_KEY_KEYPAD_0] = KEY_KP0,
    [VKM_KEY_KEYPAD_DECIMAL] = KEY_KPDOT,
    [VKM_KEY_KEYPAD_COMMA] = KEY_KPCOMMA,
};

struct vkm_context {
    struct libevdev *dev;
    struct libevdev_uinput *uidev;

    char *name;
    bool mouse_hi_res_scroll;
    bool mouse_horizontal_scroll;

    /** Last modifier byte emitted (\ref vkm_key_modifiers bits 0–7). */
    uint8_t keyboard_modifier_mask;

    /** Per-key depressed state from \ref vkm_keyboard_send (non-\c NONE keys). */
    bool keyboard_key_pressed[VKM_KEY_MAX];

    // Existing state of buttons
    struct vkm_mouse_button_state mouse_buttons;
};

static const int mouse_button_map[VKM_MOUSE_BTN_MAX] = {
    [VKM_MOUSE_BTN_LEFT]    = BTN_LEFT,
    [VKM_MOUSE_BTN_RIGHT]   = BTN_RIGHT,
    [VKM_MOUSE_BTN_MIDDLE]  = BTN_MIDDLE,
};

static const int button_value_map[VKM_BUTTON_STATE_MAX] = {
    [VKM_BUTTON_PRESSED]  = 1,
    [VKM_BUTTON_RELEASED] = 0,
};

/*
 * High-resolution scroll uses the same scale as REL_WHEEL_HI_RES in the kernel
 * (120 units per “click” / detent; aligns with libinput and the Windows convention).
 */
#define VKM_SCROLL_HI_RES_PER_DETENT 120

vkm_result vkm_init(vkm_context **ctx)
{
    char name_buf[32];
    char *name;
    vkm_result rc;
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    *ctx = calloc(1, sizeof(**ctx));
    if (*ctx == NULL) {
        return VKM_ERROR_OUT_OF_MEMORY;
    }

    rc = VKM_SUCCESS;
    // Create a default name for the virtual device, this will be replaced when
    // the user calls vkm_set_option with the appropriate name field.
    snprintf(name_buf, sizeof(name_buf), "VKM virtual device @%08" PRIx64,
            (uint64_t)(time(NULL)));
    name = strdup(name_buf);
    if (name == NULL) {
        rc = VKM_ERROR_OUT_OF_MEMORY;
        goto error;
    }
    (*ctx)->name = name;

    // Set defaults for the flags
    return VKM_SUCCESS;

error:
    if (name) {
        free(name);
    }
    free(*ctx);
    *ctx = NULL;
    return rc;
}

void vkm_exit(vkm_context *ctx)
{
    volatile unsigned char *vp;

    if (ctx == NULL) {
        return;
    }

    (void)vkm_reset(ctx);

    free(ctx->name);

    if (ctx->uidev) {
        libevdev_uinput_destroy(ctx->uidev);
    }

    if (ctx->dev) {
        libevdev_free(ctx->dev);
    }

    /* Clear the memory to prevent reuse */
    vp = (volatile unsigned char *)ctx;
    for (int i = 0; i < sizeof(*ctx); i++) {
        vp[i] = (unsigned char)0;
    }

    free(ctx);
}

// Enable mouse events
static void enable_mouse_events(vkm_context *ctx)
{
    libevdev_enable_event_type(ctx->dev, EV_REL);
    libevdev_enable_event_code(ctx->dev, EV_REL, REL_X, NULL);
    libevdev_enable_event_code(ctx->dev, EV_REL, REL_Y, NULL);
    libevdev_enable_event_code(ctx->dev, EV_REL, REL_WHEEL, NULL);
    if (ctx->mouse_hi_res_scroll) {
        libevdev_enable_event_code(ctx->dev, EV_REL, REL_WHEEL_HI_RES, NULL);
    }

    if (ctx->mouse_horizontal_scroll) {
        libevdev_enable_event_code(ctx->dev, EV_REL, REL_HWHEEL, NULL);
        if (ctx->mouse_hi_res_scroll) {
            libevdev_enable_event_code(ctx->dev, EV_REL, REL_HWHEEL_HI_RES, NULL);
        }
    }

    libevdev_enable_event_type(ctx->dev, EV_KEY);
    libevdev_enable_event_code(ctx->dev, EV_KEY, BTN_LEFT, NULL);
    libevdev_enable_event_code(ctx->dev, EV_KEY, BTN_RIGHT, NULL);
    libevdev_enable_event_code(ctx->dev, EV_KEY, BTN_MIDDLE, NULL);
}

static void enable_keyboard_events(vkm_context *ctx)
{
    vkm_key k;

    for (k = (vkm_key)0; k < VKM_KEY_MAX; k++) {
        int code = vkm_key_to_evdev[k];

        if (code >= 0) {
            libevdev_enable_event_code(ctx->dev, EV_KEY, (unsigned int)code, NULL);
        }
    }
}

static const int vkm_modifier_evdev[8] = {
    KEY_LEFTCTRL,
    KEY_LEFTSHIFT,
    KEY_LEFTALT,
    KEY_LEFTMETA,
    KEY_RIGHTCTRL,
    KEY_RIGHTSHIFT,
    KEY_RIGHTALT,
    KEY_RIGHTMETA,
};

static vkm_result apply_modifier_mask(vkm_context *ctx, uint32_t want)
{
    uint32_t have = ctx->keyboard_modifier_mask;
    uint32_t diff = want ^ have;
    unsigned bit;

    if (diff == 0) {
        return VKM_SUCCESS;
    }

    for (bit = 0; bit < 8; bit++) {
        uint32_t b = (uint32_t)1 << bit;

        if (diff & b) {
            int val = (want & b) ? 1 : 0;
            int rc;

            rc = libevdev_uinput_write_event(ctx->uidev, EV_KEY, vkm_modifier_evdev[bit], val);
            if (rc != 0) {
                return VKM_ERROR_EVENT;
            }
        }
    }

    ctx->keyboard_modifier_mask = (uint8_t)want;
    return VKM_SUCCESS;
}

vkm_result vkm_start(vkm_context *ctx)
{
    int rc;
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (ctx->uidev) {
        // Already initialized, no need to do anything else
        return VKM_SUCCESS;
    }

    if (!(ctx->dev)) {
        // This will happen on the first time this function is called, but
        // will get cleared up if the uinput device is not created, so this
        // is just a safety check.
        ctx->dev = libevdev_new();
        if (ctx->dev == NULL) {
            return VKM_ERROR_DEV_FAILURE;
        }

        libevdev_set_name(ctx->dev, ctx->name);
        enable_mouse_events(ctx);
        enable_keyboard_events(ctx);
    }

    rc = libevdev_uinput_create_from_device(ctx->dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
            &(ctx->uidev));
    if (rc != 0) {
        goto error;
    }
    return VKM_SUCCESS;

error:
    libevdev_free(ctx->dev);
    ctx->dev = NULL;
    return VKM_ERROR_DEV_FAILURE;
}

bool vkm_is_ready(vkm_context *ctx) {
    if (ctx == NULL) {
        return false;
    }

    return (ctx->dev != NULL && ctx->uidev != NULL);
}

bool vkm_platform_supported(void)
{
    return true;
}

bool vkm_feature_supported(vkm_feature feat)
{
    switch (feat) {
        case VKM_FEAT_MOUSE:
            return true;

        case VKM_FEAT_KEYBOARD:
        case VKM_FEAT_KEYBOARD_MODIFIERS:
            return true;

        default:
            return false;
    }
}

vkm_result vkm_set_option(vkm_context *ctx, vkm_option option, ...)
{
    va_list ap;
    bool flag;
    char *name;
    char *name2;
    vkm_result rc;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    va_start(ap, option);
    rc = VKM_SUCCESS;
    switch (option) {
        case VKM_OPT_HI_RES_SCROLL:
            // read as int, as bool is promoted to int when passed as a va_arg
            flag = va_arg(ap, int);
            ctx->mouse_hi_res_scroll = (bool)flag;
            break;

        case VKM_OPT_HORIZONTAL_SCROLL:
            // read as int, as bool is promoted to int when passed as a va_arg
            flag = va_arg(ap, int);
            ctx->mouse_horizontal_scroll = (bool)flag;
            break;

        case VKM_OPT_DEVICE_NAME:
            name = va_arg(ap, char *);
            name2 = strdup(name);
            if (name2 == NULL) {
                rc = VKM_ERROR_OUT_OF_MEMORY;
            } else {
                free(ctx->name);
                ctx->name = name2;
            }
            break;

        default:
            rc = VKM_ERROR_INVALID_PARAM;
            break;
    }

    va_end(ap);
    return rc;
}

vkm_result vkm_mouse_move(vkm_context *ctx, int dx, int dy)
{
    int rc;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    if (!dx && !dy) {
        return VKM_ERROR_NO_CHANGE;
    }

    if (dx) {
        rc = libevdev_uinput_write_event(ctx->uidev, EV_REL, REL_X, dx);
        if (rc != 0) {
            return VKM_ERROR_EVENT;
        }
    }

    if (dy) {
        rc = libevdev_uinput_write_event(ctx->uidev, EV_REL, REL_Y, dy);
        if (rc != 0) {
            return VKM_ERROR_EVENT;
        }
    }

    return VKM_SUCCESS;
}

vkm_result vkm_mouse_click(vkm_context *ctx, vkm_mouse_button button, vkm_button_state state)
{
    int rc;
    vkm_button_state existing;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (button >= VKM_MOUSE_BTN_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (state >= VKM_BUTTON_STATE_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    existing = _vkm_get_mouse_button_state(&(ctx->mouse_buttons), button);
    if (existing == state) {
        return VKM_ERROR_NO_CHANGE;
    }
    _vkm_set_mouse_button_state(&(ctx->mouse_buttons), button, state);

    rc = libevdev_uinput_write_event(ctx->uidev, EV_KEY,
            mouse_button_map[button], button_value_map[state]);
    if (rc != 0) {
        return VKM_ERROR_EVENT;
    }

    return VKM_SUCCESS;
}

vkm_result vkm_mouse_scroll(vkm_context *ctx, vkm_mouse_scroll_direction dir)
{
    int rc;
    int sign;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (dir >= VKM_MOUSE_SCROLL_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    if (dir == VKM_MOUSE_SCROLL_LEFT || dir == VKM_MOUSE_SCROLL_RIGHT) {
        if (!ctx->mouse_horizontal_scroll) {
            return VKM_ERROR_NOT_SUPPORTED;
        }
        sign = (dir == VKM_MOUSE_SCROLL_LEFT) ? -1 : 1;
        if (ctx->mouse_hi_res_scroll) {
            rc = libevdev_uinput_write_event(ctx->uidev, EV_REL, REL_HWHEEL_HI_RES,
                    sign * VKM_SCROLL_HI_RES_PER_DETENT);
            if (rc != 0) {
                return VKM_ERROR_EVENT;
            }
        }
        rc = libevdev_uinput_write_event(ctx->uidev, EV_REL, REL_HWHEEL, sign);
        if (rc != 0) {
            return VKM_ERROR_EVENT;
        }
        return VKM_SUCCESS;
    }

    sign = (dir == VKM_MOUSE_SCROLL_UP) ? 1 : -1;
    if (ctx->mouse_hi_res_scroll) {
        rc = libevdev_uinput_write_event(ctx->uidev, EV_REL, REL_WHEEL_HI_RES,
                sign * VKM_SCROLL_HI_RES_PER_DETENT);
        if (rc != 0) {
            return VKM_ERROR_EVENT;
        }
    }
    rc = libevdev_uinput_write_event(ctx->uidev, EV_REL, REL_WHEEL, sign);
    if (rc != 0) {
        return VKM_ERROR_EVENT;
    }

    return VKM_SUCCESS;
}

vkm_result vkm_keyboard_send(vkm_context *ctx, vkm_key key, vkm_key_modifiers modifiers,
                             vkm_key_state state)
{
    vkm_result res;
    int evcode;
    int val;
    uint32_t modmask = (uint32_t)modifiers & 0xffu;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (key >= VKM_KEY_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (state >= VKM_KEY_STATE_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    val = (state == VKM_KEY_STATE_PRESSED) ? 1 : 0;

    if (state == VKM_KEY_STATE_PRESSED) {
        res = apply_modifier_mask(ctx, modmask);
        if (res != VKM_SUCCESS) {
            return res;
        }

        if (key == VKM_KEY_NONE) {
            return VKM_SUCCESS;
        }

        evcode = vkm_key_to_evdev[key];
        if (evcode < 0) {
            return VKM_ERROR_INVALID_PARAM;
        }

        if (libevdev_uinput_write_event(ctx->uidev, EV_KEY, (unsigned int)evcode, val) != 0) {
            return VKM_ERROR_EVENT;
        }

        ctx->keyboard_key_pressed[key] = true;
    } else {
        if (key != VKM_KEY_NONE) {
            evcode = vkm_key_to_evdev[key];
            if (evcode < 0) {
                return VKM_ERROR_INVALID_PARAM;
            }

            if (libevdev_uinput_write_event(ctx->uidev, EV_KEY, (unsigned int)evcode, val) != 0) {
                return VKM_ERROR_EVENT;
            }

            ctx->keyboard_key_pressed[key] = false;
        }

        res = apply_modifier_mask(ctx, modmask);
        if (res != VKM_SUCCESS) {
            return res;
        }
    }

    return VKM_SUCCESS;
}

static void keyboard_state_clear_tracking(vkm_context *ctx)
{
    vkm_mouse_button b;

    memset(ctx->keyboard_key_pressed, 0, sizeof(ctx->keyboard_key_pressed));
    ctx->keyboard_modifier_mask = 0;
    for (b = 0; b < VKM_MOUSE_BTN_MAX; b++) {
        _vkm_set_mouse_button_state(&ctx->mouse_buttons, b, VKM_BUTTON_RELEASED);
    }
}

vkm_result vkm_reset(vkm_context *ctx)
{
    vkm_key k;
    vkm_mouse_button b;
    int rc;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        keyboard_state_clear_tracking(ctx);
        return VKM_SUCCESS;
    }

    for (k = (vkm_key)0; k < VKM_KEY_MAX; k++) {
        if (ctx->keyboard_key_pressed[k]) {
            int code = vkm_key_to_evdev[k];

            if (code >= 0) {
                rc = libevdev_uinput_write_event(ctx->uidev, EV_KEY, (unsigned int)code, 0);
                if (rc != 0) {
                    return VKM_ERROR_EVENT;
                }
            }

            ctx->keyboard_key_pressed[k] = false;
        }
    }

    rc = apply_modifier_mask(ctx, 0);
    if (rc != VKM_SUCCESS) {
        return rc;
    }

    for (b = 0; b < VKM_MOUSE_BTN_MAX; b++) {
        if (_vkm_get_mouse_button_state(&ctx->mouse_buttons, b) == VKM_BUTTON_PRESSED) {
            rc = libevdev_uinput_write_event(ctx->uidev, EV_KEY,
                    mouse_button_map[b], button_value_map[VKM_BUTTON_RELEASED]);
            if (rc != 0) {
                return VKM_ERROR_EVENT;
            }

            _vkm_set_mouse_button_state(&ctx->mouse_buttons, b, VKM_BUTTON_RELEASED);
        }
    }

    return vkm_sync(ctx);
}

vkm_result vkm_sync(vkm_context *ctx)
{
    int rc;
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    rc = libevdev_uinput_write_event(ctx->uidev, EV_SYN, SYN_REPORT, 0);
    if (rc != 0) {
        return VKM_ERROR_EVENT;
    }

    return VKM_SUCCESS;
}
