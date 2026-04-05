/*
 * Saitek X52 Pro MFD & LED driver - keyboard layout from config
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_KEYBOARD_LAYOUT_H
#define X52D_KEYBOARD_LAYOUT_H

#include <daemon/layout_format.h>

/**
 * @brief Load or reload layout from @c profile_keyboard_layout (@c Profiles.KeyboardLayout).
 *
 * Resolves @c $datadir/x52d/<basename>.x52l with basename hardening; falls back to @c us once if the
 * requested file is missing. Exits the process if no layout can be loaded.
 */
void x52d_keyboard_layout_reload(char *profile_keyboard_layout_value);

/** Active layout after @ref x52d_keyboard_layout_reload, or @c NULL before the first apply. */
const x52_layout *x52d_keyboard_layout_get(void);

void x52d_keyboard_layout_fini(void);

#endif /* X52D_KEYBOARD_LAYOUT_H */
