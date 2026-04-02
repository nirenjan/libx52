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
#include "libx52io.h"

extern volatile bool mouse_isometric_mode;
extern volatile int mouse_scroll_dir;
extern volatile int mouse_curve_factor;
extern volatile int mouse_deadzone_factor;
extern volatile int mouse_sensitivity;

void x52d_mouse_thread_control(bool enabled);
void x52d_mouse_handler_init(void);
void x52d_mouse_handler_exit(void);
void x52d_mouse_report_event(libx52io_report *report);

#endif // !defined X52D_MOUSE_H
