/*
 * Saitek X52 Pro MFD & LED driver - Virtual keyboard
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_KEYBOARD_H
#define X52D_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @file x52d_keyboard.h
 * @brief Virtual keyboard (uinput) for profile-generated key events
 */

#if defined(HAVE_EVDEV)

/**
 * @brief Initialize the virtual keyboard device.
 *
 * Creates a uinput keyboard that can emit key events. Must be called
 * before x52d_keyboard_evdev_key().
 */
void x52d_keyboard_evdev_init(void);

/**
 * @brief Shut down and destroy the virtual keyboard device.
 */
void x52d_keyboard_evdev_exit(void);

/**
 * @brief Emit a key event (down or up).
 *
 * @param key_code Linux input event key code (e.g. KEY_A).
 * @param value    1 for key down, 0 for key up.
 * @return 0 on success, negative errno on failure.
 */
int x52d_keyboard_evdev_key(uint16_t key_code, int value);

/**
 * @brief Return whether the virtual keyboard is available.
 */
bool x52d_keyboard_evdev_available(void);

#else

static inline void x52d_keyboard_evdev_init(void) { (void)0; }
static inline void x52d_keyboard_evdev_exit(void) { (void)0; }
static inline int x52d_keyboard_evdev_key(uint16_t key_code, int value) {
    (void)key_code;
    (void)value;
    return 0;
}
static inline bool x52d_keyboard_evdev_available(void) { return false; }

#endif /* HAVE_EVDEV */

#endif /* !defined X52D_KEYBOARD_H */
