/*
 * Saitek X52 Pro MFD & LED driver — keyboard layout (.layout) API
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_LAYOUT_H
#define X52D_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vkm.h"

/** Loaded keyboard layout: maps Unicode code points to HID key + modifier byte */
struct x52d_layout;

/** One keystroke recipe for \ref vkm_keyboard_send */
struct x52d_layout_recipe {
    vkm_key key;
    vkm_key_modifiers mods;
};

/**
 * @brief Load a layout from an INI file (see x52d_layout.c header for grammar).
 *
 * @param[out]  out     Receives the new layout; must be released with
 *                      \ref x52d_layout_free. Undefined on failure.
 * @param[in]   path    Filesystem path
 *
 * @returns 0 on success, EINVAL on parse/validation error, ENOMEM, or EIO
 */
int x52d_layout_load_file(struct x52d_layout **out, const char *path);

/**
 * @brief Load a layout from a memory buffer (NUL-terminated \p data is optional;
 * length is given explicitly).
 */
int x52d_layout_load_buffer(struct x52d_layout **out, const char *data, size_t len);

void x52d_layout_free(struct x52d_layout *layout);

const char *x52d_layout_get_name(const struct x52d_layout *layout);

/**
 * @brief Look up the recipe for a Unicode code point.
 *
 * @returns true if found
 */
bool x52d_layout_lookup(const struct x52d_layout *layout, uint32_t cp,
                        struct x52d_layout_recipe *out_recipe);

#endif /* X52D_LAYOUT_H */
