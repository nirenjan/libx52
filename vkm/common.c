/*
 * VKM common functions
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 *
 * SPDX-LicenseIdentifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdio.h>
#include <stdlib.h>

#include "gettext.h"

#include "vkm-internal.h"

#define N_(str) gettext_noop(str)
#define _(str)  dgettext(PACKAGE, str)

/* Error buffer used for building custom error strings */
static char error_buffer[256];

/* List of error strings (indices must match \ref vkm_error_code) */
static const char *error_string[] = {
    N_("Success"),
    N_("Unknown error"),
    N_("Not ready"),
    N_("Out of memory"),
    N_("Invalid parameter"),
    N_("Not supported"),
    N_("Virtual device failure"),
    N_("Unable to write event"),
    N_("No state change"),
};

const char *vkm_strerror(vkm_result code)
{
    switch ((vkm_error_code)code) {
    case VKM_SUCCESS:
    case VKM_ERROR_UNKNOWN:
    case VKM_ERROR_NOT_READY:
    case VKM_ERROR_OUT_OF_MEMORY:
    case VKM_ERROR_INVALID_PARAM:
    case VKM_ERROR_NOT_SUPPORTED:
    case VKM_ERROR_DEV_FAILURE:
    case VKM_ERROR_EVENT:
    case VKM_ERROR_NO_CHANGE:
        return _(error_string[code]);

    default:
        snprintf(error_buffer, sizeof(error_buffer),
                 _("Unknown error %d"), (int)code);
        return error_buffer;
    }
}

vkm_button_state _vkm_get_mouse_button_state(struct vkm_mouse_button_state *state, vkm_mouse_button button)
{
    if (state == NULL) {
        return false;
    }

    return state->pressed[button];
}

void _vkm_set_mouse_button_state(struct vkm_mouse_button_state *sstate, vkm_mouse_button button, vkm_button_state state)
{
    if (sstate != NULL) {
        sstate->pressed[button] = state;
    }
}
