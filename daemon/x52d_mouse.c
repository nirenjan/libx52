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

// Mouse speed is the delay between subsequent mouse reports
#define DEFAULT_MOUSE_SPEED 250000

volatile bool mouse_enabled = false;
volatile int mouse_delay = DEFAULT_MOUSE_SPEED;

#define MAX_MOUSE_SPEED 5
static const int mouse_speed_map[MAX_MOUSE_SPEED] = {
    250000,
    200000,
    150000,
    100000,
     50000,
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
