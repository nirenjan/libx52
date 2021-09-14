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

// Mouse speed is the delay between subsequent mouse reports
#define DEFAULT_MOUSE_SPEED 250000

static bool mouse_enabled = false;
static int mouse_speed = DEFAULT_MOUSE_SPEED;

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
    mouse_enabled = enabled;
}

void x52d_cfg_set_Mouse_Speed(int speed)
{
    int new_speed;
    if (speed >= 0 && speed < MAX_MOUSE_SPEED) {
        new_speed = mouse_speed_map[speed];
        PINELOG_DEBUG(_("Setting mouse speed to %d (delay %d ms)"),
                      speed, new_speed);
        mouse_speed = new_speed;
    } else {
        PINELOG_INFO(_("Ignoring mouse speed %d outside supported range (0-%d)"),
                     speed, MAX_MOUSE_SPEED);
    }
}
