/*
 * Saitek X52 Pro MFD & LED driver - Profile (button-to-keyboard mapping)
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_PROFILE_H
#define X52D_PROFILE_H

#include "libx52io.h"

/**
 * @file x52d_profile.h
 * @brief Profile loading and application: map joystick buttons to keyboard events
 */

#if defined(HAVE_EVDEV)

/**
 * @brief Initialize the profile module and load the active profile.
 *
 * Reads Profiles.Directory and Profiles.Profile from config, loads
 * the profile file, and resolves ShiftButton from the [Profile] section. If loading
 * fails, the profile is empty (no mappings).
 */
void x52d_profile_init(void);

/**
 * @brief Shut down the profile module and free resources.
 */
void x52d_profile_exit(void);

/**
 * @brief Return the profile display name from the loaded profile file.
 *
 * The name is set in the profile file with [Profile] Name = ...
 * @return The profile name, or NULL if none set or no profile loaded.
 *         Valid until x52d_profile_exit() or next x52d_profile_init().
 */
const char *x52d_profile_get_name(void);

/**
 * @brief Apply profile mappings to button state changes.
 *
 * Compares @a report with @a prev and emits keyboard events for any
 * button that has a mapping in the current (mode, shift) layer.
 * Single-key mappings: key down on press, key up on release.
 * Macro mappings: sequence of key down/up on button down only.
 *
 * @param report Current joystick report.
 * @param prev   Previous report (for edge detection).
 */
void x52d_profile_apply(const libx52io_report *report,
                       const libx52io_report *prev);

#else

static inline void x52d_profile_init(void) { (void)0; }
static inline void x52d_profile_exit(void) { (void)0; }
static inline const char *x52d_profile_get_name(void) { return NULL; }
static inline void x52d_profile_apply(const libx52io_report *report,
                                     const libx52io_report *prev) {
    (void)report;
    (void)prev;
}

#endif /* HAVE_EVDEV */

#endif /* !defined X52D_PROFILE_H */
