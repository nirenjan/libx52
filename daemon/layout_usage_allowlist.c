/*
 * Saitek X52 Pro MFD & LED driver - Keyboard layout HID usage allowlist
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <daemon/layout_usage_allowlist.h>

#include <stdint.h>

#include <vkm/vkm.h>

/* layout binary and compiler must stay aligned with vkm_key numeric values. */
_Static_assert((unsigned)VKM_KEY_A == 0x04u, "vkm_key main block start");
_Static_assert((unsigned)VKM_KEY_CAPS_LOCK == 0x39u, "vkm_key main block end");
_Static_assert((unsigned)VKM_KEY_INTL_BACKSLASH == 0x64u, "vkm_key ISO backslash");

bool x52_layout_usage_key_allowed(uint8_t usage)
{
    if (usage == 0) {
        return false;
    }
    if (usage >= 0x04 && usage <= 0x39) {
        return true;
    }
    if (usage == 0x64) {
        return true;
    }
    return false;
}
