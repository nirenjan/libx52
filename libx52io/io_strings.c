/*
 * Saitek X52 IO driver - string representations
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include "libx52io.h"
#include "gettext.h"

/* The strings corresponding to the axis and button enumerations are
 * deliberately left untranslated, as they essentially correspond one
 * on one to the enumeration definitions.
 */

/* String mapping for axis */
static const char * _x52io_axis_str[LIBX52IO_AXIS_MAX] = {
    [LIBX52IO_AXIS_X] = "ABS_X",
    [LIBX52IO_AXIS_Y] = "ABS_Y",
    [LIBX52IO_AXIS_RZ] = "ABS_RZ",
    [LIBX52IO_AXIS_Z] = "ABS_Z",
    [LIBX52IO_AXIS_RX] = "ABS_RX",
    [LIBX52IO_AXIS_RY] = "ABS_RY",
    [LIBX52IO_AXIS_SLIDER] = "ABS_SLIDER",
    [LIBX52IO_AXIS_THUMBX] = "ABS_THUMBX",
    [LIBX52IO_AXIS_THUMBY] = "ABS_THUMBY",
    [LIBX52IO_AXIS_HATX] = "ABS_HATX",
    [LIBX52IO_AXIS_HATY] = "ABS_HATY",
};

const char * libx52io_axis_to_str(libx52io_axis axis)
{
    if (axis >= LIBX52IO_AXIS_X && axis < LIBX52IO_AXIS_MAX) {
        return _x52io_axis_str[axis];
    }

    return NULL;
}

/* String mapping for button */
static const char * _x52io_button_str[LIBX52IO_BUTTON_MAX] = {
    [LIBX52IO_BTN_TRIGGER] = "BTN_TRIGGER",
    [LIBX52IO_BTN_TRIGGER_2] = "BTN_TRIGGER_2",
    [LIBX52IO_BTN_FIRE] = "BTN_FIRE",
    [LIBX52IO_BTN_PINKY] = "BTN_PINKY",
    [LIBX52IO_BTN_A] = "BTN_A",
    [LIBX52IO_BTN_B] = "BTN_B",
    [LIBX52IO_BTN_C] = "BTN_C",
    [LIBX52IO_BTN_D] = "BTN_D",
    [LIBX52IO_BTN_E] = "BTN_E",
    [LIBX52IO_BTN_T1_UP] = "BTN_T1_UP",
    [LIBX52IO_BTN_T1_DN] = "BTN_T1_DN",
    [LIBX52IO_BTN_T2_UP] = "BTN_T2_UP",
    [LIBX52IO_BTN_T2_DN] = "BTN_T2_DN",
    [LIBX52IO_BTN_T3_UP] = "BTN_T3_UP",
    [LIBX52IO_BTN_T3_DN] = "BTN_T3_DN",
    [LIBX52IO_BTN_POV_1_N] = "BTN_POV_1_N",
    [LIBX52IO_BTN_POV_1_E] = "BTN_POV_1_E",
    [LIBX52IO_BTN_POV_1_S] = "BTN_POV_1_S",
    [LIBX52IO_BTN_POV_1_W] = "BTN_POV_1_W",
    [LIBX52IO_BTN_POV_2_N] = "BTN_POV_2_N",
    [LIBX52IO_BTN_POV_2_E] = "BTN_POV_2_E",
    [LIBX52IO_BTN_POV_2_S] = "BTN_POV_2_S",
    [LIBX52IO_BTN_POV_2_W] = "BTN_POV_2_W",
    [LIBX52IO_BTN_CLUTCH] = "BTN_CLUTCH",
    [LIBX52IO_BTN_MOUSE_PRIMARY] = "BTN_MOUSE_PRIMARY",
    [LIBX52IO_BTN_MOUSE_SECONDARY] = "BTN_MOUSE_SECONDARY",
    [LIBX52IO_BTN_MOUSE_SCROLL_UP] = "BTN_MOUSE_SCROLL_UP",
    [LIBX52IO_BTN_MOUSE_SCROLL_DN] = "BTN_MOUSE_SCROLL_DN",
    [LIBX52IO_BTN_FUNCTION] = "BTN_FUNCTION",
    [LIBX52IO_BTN_START_STOP] = "BTN_START_STOP",
    [LIBX52IO_BTN_RESET] = "BTN_RESET",
    [LIBX52IO_BTN_PG_UP] = "BTN_PG_UP",
    [LIBX52IO_BTN_PG_DN] = "BTN_PG_DN",
    [LIBX52IO_BTN_UP] = "BTN_UP",
    [LIBX52IO_BTN_DN] = "BTN_DN",
    [LIBX52IO_BTN_SELECT] = "BTN_SELECT",
    [LIBX52IO_BTN_MODE_1] = "BTN_MODE_1",
    [LIBX52IO_BTN_MODE_2] = "BTN_MODE_2",
    [LIBX52IO_BTN_MODE_3] = "BTN_MODE_3",
};

const char * libx52io_button_to_str(libx52io_button button)
{
    if (button >= LIBX52IO_BTN_TRIGGER && button < LIBX52IO_BUTTON_MAX) {
        return _x52io_button_str[button];
    }

    return NULL;
}

/* Error buffer used for building custom error strings */
static char error_buffer[256];

#define N_(str) gettext_noop(str)

static const char *error_string[] = {
    N_("Success"),
    N_("Initialization failure"),
    N_("No device"),
    N_("Invalid arguments"),
    N_("Connection failure"),
    N_("I/O error"),
    N_("Read timeout"),
};

#define _(str) dgettext(PACKAGE, str)

const char * libx52io_strerror(libx52io_error_code code)
{
    switch (code) {
    case LIBX52IO_SUCCESS:
    case LIBX52IO_ERROR_INIT_FAILURE:
    case LIBX52IO_ERROR_NO_DEVICE:
    case LIBX52IO_ERROR_INVALID:
    case LIBX52IO_ERROR_CONN:
    case LIBX52IO_ERROR_IO:
    case LIBX52IO_ERROR_TIMEOUT:
        return _(error_string[code]);

    default:
        snprintf(error_buffer, sizeof(error_buffer), _("Unknown error %d"), code);
        break;
    }

    (void)_x52io_axis_str;
    (void)_x52io_button_str;

    return error_buffer;
}

