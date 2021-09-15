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

#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

// Mouse speed is the delay in microseconds between subsequent mouse reports
#define DEFAULT_MOUSE_DELAY 70000
#define MOUSE_DELAY_DELTA   10000

volatile bool mouse_enabled = false;
volatile int mouse_delay = DEFAULT_MOUSE_DELAY;

#define MAX_MOUSE_SPEED 5
static const int mouse_speed_map[MAX_MOUSE_SPEED] = {
    DEFAULT_MOUSE_DELAY,
    DEFAULT_MOUSE_DELAY - MOUSE_DELAY_DELTA*1,
    DEFAULT_MOUSE_DELAY - MOUSE_DELAY_DELTA*2,
    DEFAULT_MOUSE_DELAY - MOUSE_DELAY_DELTA*3,
    DEFAULT_MOUSE_DELAY - MOUSE_DELAY_DELTA*4,
};

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
    if (speed >= 0 && speed < MAX_MOUSE_SPEED) {
        new_delay = mouse_speed_map[speed];
        PINELOG_DEBUG(_("Setting mouse speed to %d (delay %d ms)"),
                      speed, new_delay);
        mouse_delay = new_delay;
    } else {
        PINELOG_INFO(_("Ignoring mouse speed %d outside supported range (0-%d)"),
                     speed, MAX_MOUSE_SPEED-1);
    }
}
