/*
 * Saitek X52 Pro MFD & LED driver - keyboard layout from config
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"

#define PINELOG_MODULE X52D_MOD_KEYBOARD_LAYOUT
#include "pinelog.h"
#include <daemon/constants.h>
#include <daemon/keyboard_layout.h>
#include <daemon/layout_format.h>

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static x52_layout *active_layout;

/**
 * Normal install: @c $DATADIR/x52d/<basename>.x52l
 *
 * If @c X52D_LAYOUT_DIR is set (non-empty), load @c $X52D_LAYOUT_DIR/<basename>.x52l instead so
 * uninstalled tests can point at the Meson build directory.
 */
static int load_layout_for_basename(const char *basename, x52_layout **out)
{
    const char *layout_dir = getenv("X52D_LAYOUT_DIR");

    if (layout_dir != NULL && layout_dir[0] != '\0') {
        char path[PATH_MAX];
        int n = snprintf(path, sizeof path, "%s/%s.x52l", layout_dir, basename);
        if (n < 0) {
            return EIO;
        }
        if ((size_t)n >= sizeof path) {
            return ENAMETOOLONG;
        }
        return x52_layout_load_path(path, out);
    }

    return x52_layout_load_datadir(DATADIR, basename, out);
}

const x52_layout *x52d_keyboard_layout_get(void)
{
    return active_layout;
}

void x52d_keyboard_layout_fini(void)
{
    x52_layout_free(active_layout);
    active_layout = NULL;
}

void x52d_keyboard_layout_reload(char *profile_keyboard_layout_value)
{
    char basename[256];
    bool rejected = false;

    x52_layout_normalize_keyboard_basename(profile_keyboard_layout_value, basename, sizeof basename, &rejected);
    if (rejected) {
        PINELOG_WARN(_("Invalid Profiles.KeyboardLayout value; using default layout basename 'us'"));
    }

    x52_layout *new_layout = NULL;
    int err = load_layout_for_basename(basename, &new_layout);

    if (err != 0 && strcmp(basename, "us") != 0) {
        PINELOG_WARN(
            _("Keyboard layout '%s' could not be loaded (%s); loading default 'us'"),
            basename, strerror(err > 0 ? err : EIO));
        err = load_layout_for_basename("us", &new_layout);
    }

    if (err != 0) {
        PINELOG_FATAL(_("Could not load keyboard layout from %s/x52d (%s)"), DATADIR,
                      strerror(err > 0 ? err : EIO));
    }

    x52_layout *old_layout = active_layout;
    active_layout = new_layout;
    x52_layout_free(old_layout);

    const char *desc = x52_layout_description(active_layout);
    PINELOG_INFO(_("Keyboard layout ready: %s (%s)"), x52_layout_name(active_layout),
                 desc[0] != '\0' ? desc : _("no description"));
}

void x52d_cfg_set_Profiles_KeyboardLayout(char *param)
{
    x52d_keyboard_layout_reload(param);
}
