/*
 * Saitek X52 Pro MFD & LED driver - Keyboard layout HID usage allowlist
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file layout_usage_allowlist.h
 * @brief HID keyboard/page (0x07) usages permitted as layout chord \c usage_key bytes.
 *
 * The set is the USB HID "main block" (usages @c 0x04-@c 0x39, i.e. @c VKM_KEY_A
 * through @c VKM_KEY_CAPS_LOCK) plus @c VKM_KEY_INTL_BACKSLASH (@c 0x64). It excludes
 * @c VKM_KEY_NONE, modifiers, function row, navigation cluster, keypad, and all other
 * @ref vkm_key values; same rule as @c tools/x52compile_layout.py.
 */
#ifndef X52D_LAYOUT_USAGE_ALLOWLIST_H
#define X52D_LAYOUT_USAGE_ALLOWLIST_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return whether @p usage may appear as the non-modifier byte in a layout entry.
 *
 * @param usage     HID usage ID (page 0x07), same encoding as @ref vkm_key.
 *
 * @returns true if @p usage is in the main-block allowlist; false for @c VKM_KEY_NONE
 *          and for any disallowed usage.
 */
bool x52_layout_usage_key_allowed(uint8_t usage);

#ifdef __cplusplus
}
#endif

#endif /* X52D_LAYOUT_USAGE_ALLOWLIST_H */
