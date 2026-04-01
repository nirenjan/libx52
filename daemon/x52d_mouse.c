/*
 * Saitek X52 Pro MFD & LED driver - Mouse driver
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define PINELOG_MODULE X52D_MOD_MOUSE
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

// Mouse speed is the delay in microseconds between subsequent mouse reports
#define DEFAULT_MOUSE_DELAY 70000
#define MOUSE_DELAY_DELTA   5000
#define MOUSE_DELAY_MIN     10000
#define MOUSE_MULT_FACTOR  4
#define MAX_MOUSE_MULT 5
#define MIN_SENSITIVITY 10
#define MAX_SENSITIVITY 500

static const double MOUSE_CURVE_FACTORS[5] = {
    1.0, 1.2, 1.5, 1.8, 2.2
};

static const double MOUSE_DEADZONES[12] = {
    0.0, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5
};

volatile int mouse_scroll_dir = 1;
volatile bool mouse_isometric_mode = false;
volatile double mouse_curve_factor = MOUSE_CURVE_FACTORS[2]; // default
volatile double mouse_deadzone = MOUSE_DEADZONES[0];
volatile int mouse_sensitivity = 0;

static int clamp_int(const char *description, int value, int min, int max)
{
    if (value < min) {
        PINELOG_DEBUG(_("Clamping %s value %d to range [%d..%d]"),
                description, value, min, max);
        return min;
    }

    if (value > max) {
        PINELOG_DEBUG(_("Clamping %s value %d to range [%d..%d]"),
                description, value, min, max);
        return max;
    }

    return value;
}

void x52d_cfg_set_Mouse_Enabled(bool enabled)
{
    PINELOG_DEBUG(_("Setting mouse enable to %s"),
                  enabled ? _("on") : _("off"));
    x52d_mouse_thread_control(enabled);
}

void x52d_cfg_set_Mouse_Speed(int speed)
{
    // DEPRECATED, calculate the sensitivity instead
    int new_delay;
    int new_mult;

    int max_base_speed = (DEFAULT_MOUSE_DELAY - MOUSE_DELAY_MIN) / MOUSE_DELAY_DELTA;
    int max_speed = max_base_speed + MAX_MOUSE_MULT * MOUSE_MULT_FACTOR;

    double sensitivity;

    if (mouse_sensitivity == 0) {
        PINELOG_WARN(_("Config option 'mouse.speed' is DEPRECATED. Please use 'mouse.sensitivity' instead"));
    }

    speed = clamp_int("mouse speed", speed, 0, max_speed);
    if (speed <= max_base_speed) {
        new_delay = DEFAULT_MOUSE_DELAY - speed * MOUSE_DELAY_DELTA;
        new_mult = MOUSE_MULT_FACTOR;
    } else {
        // speed between max_base_speed & max_speed
        new_delay = MOUSE_DELAY_MIN;
        new_mult = MOUSE_MULT_FACTOR + (speed - max_base_speed);
    }

    sensitivity = round(10000000.0 / new_delay * new_mult / (double)MOUSE_MULT_FACTOR);

    PINELOG_INFO(_("Migrating legacy mouse speed '%d' to sensitivity '%d%%'"),
                 speed, (int)sensitivity);
    mouse_sensitivity = clamp_int(_("speed -> sensitivity"), (int)sensitivity,
                                  MIN_SENSITIVITY, MAX_SENSITIVITY);
}

void x52d_cfg_set_Mouse_ReverseScroll(bool enabled)
{
    PINELOG_DEBUG(_("Setting mouse reverse scroll to %s"),
                  enabled ? _("on") : _("off"));

    if (enabled) {
        mouse_scroll_dir = -1;
    } else {
        mouse_scroll_dir = 1;
    }
}

void x52d_config_set_Mouse_IsometricMode(bool enabled)
{
    PINELOG_DEBUG(_("Setting mouse isometric mode to %s"),
                  enabled ? _("on") : _("off"));
    mouse_isometric_mode = enabled;
}

void x52d_config_set_Mouse_Sensitivity(int factor)
{
    mouse_sensitivity = clamp_int(_("sensitivity"), factor,
                                  MIN_SENSITIVITY, MAX_SENSITIVITY);

    PINELOG_DEBUG(_("Setting mouse sensitivity to %d%%"), mouse_sensitivity);
}

void x52d_config_set_Mouse_CurveFactor(int factor)
{
    // Factor ranges from 1-5, clamp it in this range
    factor = clamp_int(_("curve factor"), factor, 1, 5);

    mouse_curve_factor = MOUSE_CURVE_FACTORS[factor-1];
    PINELOG_DEBUG(_("Setting mouse curve factor to %f"), mouse_curve_factor);
}

void x52d_config_set_Mouse_Deadzone(int factor)
{
    // Factor ranges from 0-12, clamp it in this range
    factor = clamp_int(_("deadzone factor"), factor, 0, 11);

    mouse_deadzone = MOUSE_DEADZONES[factor];
    PINELOG_DEBUG(_("Setting mouse deadzone to %f"), mouse_deadzone);
}
