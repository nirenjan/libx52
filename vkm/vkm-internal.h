/*
 * VKM common functions
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 *
 * SPDX-LicenseIdentifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdbool.h>
#include <vkm/vkm.h>

#ifndef VKM_INTERNAL_H
#define VKM_INTERNAL_H

struct vkm_mouse_button_state {
    vkm_button_state pressed[VKM_MOUSE_BTN_MAX];
};

vkm_button_state _vkm_get_mouse_button_state(struct vkm_mouse_button_state *state, vkm_mouse_button button);
void _vkm_set_mouse_button_state(struct vkm_mouse_button_state *sstate, vkm_mouse_button button, vkm_button_state state);

#endif // !defined VKM_INTERNAL_H
