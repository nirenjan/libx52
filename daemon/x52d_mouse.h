/*
 * Saitek X52 Pro MFD & LED driver - Mouse driver
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_MOUSE_H
#define X52D_MOUSE_H

#include <stdbool.h>

extern volatile bool mouse_enabled;
extern volatile int mouse_delay;

void x52d_mouse_evdev_thread_control(bool enabled);

#endif // !defined X52D_MOUSE_H
