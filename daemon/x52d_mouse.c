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

#define PINELOG_MODULE X52D_MOD_MOUSE
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

// Mouse speed is the delay in microseconds between subsequent mouse reports
#define DEFAULT_MOUSE_DELAY 70000
#define MOUSE_DELAY_DELTA   5000
#define MOUSE_DELAY_MIN     10000
#define MAX_MOUSE_MULT 5

volatile bool mouse_enabled = false;
volatile int mouse_delay = DEFAULT_MOUSE_DELAY;
volatile int mouse_mult = MOUSE_MULT_FACTOR;

void x52d_cfg_set_Mouse_Enabled(bool enabled)
{
    PINELOG_DEBUG(_("Setting mouse enable to %s"),
                  enabled ? _("on") : _("off"));
    #if defined HAVE_EVDEV
    x52d_mouse_evdev_thread_control(enabled);
    #endif
}

void x52d_cfg_set_Mouse_Speed(int speed)
{
    int new_delay;
    int new_mult;

    int max_base_speed = (DEFAULT_MOUSE_DELAY - MOUSE_DELAY_MIN) / MOUSE_DELAY_DELTA;
    int max_speed = max_base_speed + MAX_MOUSE_MULT * MOUSE_MULT_FACTOR;

    if (speed < 0 || speed > max_speed) {
        PINELOG_INFO(_("Ignoring mouse speed %d outside supported range (0-%d)"),
                     speed, max_speed);
        return;
    } else if (speed <= max_base_speed) {
        new_delay = DEFAULT_MOUSE_DELAY - speed * MOUSE_DELAY_DELTA;
        new_mult = MOUSE_MULT_FACTOR;
    } else {
        // speed between max_base_speed & max_speed
        new_delay = MOUSE_DELAY_MIN;
        new_mult = MOUSE_MULT_FACTOR + (speed - max_base_speed);
    }

    PINELOG_DEBUG(_("Setting mouse speed to %d (delay %d ms, multiplier %f)"),
                  speed, new_delay / 1000, new_mult / (double)MOUSE_MULT_FACTOR);
    mouse_delay = new_delay;
    mouse_mult = new_mult;
}
