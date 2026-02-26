/*
 * Saitek X52 Pro MFD & LED driver - Virtual keyboard (evdev/uinput)
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "libevdev/libevdev.h"
#include "libevdev/libevdev-uinput.h"
#include "x52d_const.h"
#include "x52d_keyboard.h"

#define PINELOG_MODULE X52D_MOD_KEYBOARD
#include "pinelog.h"

/* Maximum key code to enable on the virtual keyboard (covers KEY_* range). */
#ifndef KEY_MAX
#define KEY_MAX 0x2ff
#endif

static struct libevdev_uinput *kbd_uidev;
static bool kbd_uidev_created = false;

int x52d_keyboard_evdev_key(uint16_t key_code, int value)
{
    int rc;

    if (!kbd_uidev_created || kbd_uidev == NULL) {
        return -ENODEV;
    }

    rc = libevdev_uinput_write_event(kbd_uidev, EV_KEY, key_code, value);
    if (rc != 0) {
        PINELOG_ERROR(_("Error writing key event (code %u, value %d): %s"),
                      (unsigned)key_code, value, strerror(-rc));
        return rc;
    }

    rc = libevdev_uinput_write_event(kbd_uidev, EV_SYN, SYN_REPORT, 0);
    if (rc != 0) {
        PINELOG_ERROR(_("Error writing keyboard sync event"));
        return rc;
    }

    return 0;
}

bool x52d_keyboard_evdev_available(void)
{
    return kbd_uidev_created;
}

void x52d_keyboard_evdev_init(void)
{
    int rc;
    struct libevdev *dev;
    unsigned int code;

    dev = libevdev_new();
    libevdev_set_name(dev, "X52 virtual keyboard");
    libevdev_enable_event_type(dev, EV_KEY);

    for (code = 1; code <= KEY_MAX; code++) {
        libevdev_enable_event_code(dev, EV_KEY, code, NULL);
    }

    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
                                            &kbd_uidev);
    libevdev_free(dev);

    if (rc != 0) {
        PINELOG_ERROR(_("Error %d creating X52 virtual keyboard: %s"),
                      -rc, strerror(-rc));
    } else {
        kbd_uidev_created = true;
        PINELOG_INFO(_("X52 virtual keyboard created"));
    }
}

void x52d_keyboard_evdev_exit(void)
{
    kbd_uidev_created = false;
    if (kbd_uidev != NULL) {
        libevdev_uinput_destroy(kbd_uidev);
        kbd_uidev = NULL;
    }
}
