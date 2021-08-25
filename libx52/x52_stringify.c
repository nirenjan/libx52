/*
 * Saitek X52 Pro MFD & LED driver - stringification
 *
 * Copyright (C) 2012-2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>

#include "libx52.h"
#include "gettext.h"

#define N_(str) gettext_noop(str)
#define _(str)  dgettext(PACKAGE, str)

#define STRINGIFY(name, max_id, errstr, ...) \
const char * libx52_ ## name ## _to_str (libx52_ ## name param) { \
    static char invalid[256]; \
    static const char *desc[] = { __VA_ARGS__ }; \
    if (param >= 0 && param <= max_id) { \
        return _(desc[param]); \
    } \
    snprintf(invalid, sizeof(invalid), _(errstr), param); \
    return invalid; \
}

STRINGIFY(clock_id, LIBX52_CLOCK_3, N_("Unknown clock ID %d"),
    N_("primary"),
    N_("secondary"),
    N_("tertiary"),
)

STRINGIFY(clock_format, LIBX52_CLOCK_FORMAT_24HR, N_("Unknown clock format %d"),
    N_("12 hour"),
    N_("24 hour"),
)

STRINGIFY(date_format, LIBX52_DATE_FORMAT_YYMMDD, N_("Unknown date format %d"),
    N_("DD-MM-YY"),
    N_("MM-DD-YY"),
    N_("YY-MM-DD"),
)

STRINGIFY(led_state, LIBX52_LED_STATE_GREEN, N_("Unknown LED state %d"),
    N_("off"),
    N_("on"),
    N_("red"),
    N_("amber"),
    N_("green"),
)

const char * libx52_led_id_to_str(libx52_led_id id)
{
    static char invalid[256];

    switch (id) {
    case LIBX52_LED_FIRE:
        return _("Fire");

    case LIBX52_LED_A:
        return _("A");

    case LIBX52_LED_B:
        return _("B");

    case LIBX52_LED_D:
        return _("D");

    case LIBX52_LED_E:
        return _("E");

    case LIBX52_LED_T1:
        return _("T1");

    case LIBX52_LED_T2:
        return _("T2");

    case LIBX52_LED_T3:
        return _("T3");

    case LIBX52_LED_POV:
        return _("POV");

    case LIBX52_LED_CLUTCH:
        return _("Clutch");

    case LIBX52_LED_THROTTLE:
        return _("Throttle");

    default:
        snprintf(invalid, sizeof(invalid), _("Unknown LED ID %d"), id);
        return invalid;
    }
}
