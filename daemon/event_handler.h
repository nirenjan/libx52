/*
 * Saitek X52 Pro MFD & LED driver - Event handler header
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_EVENT_HANDLER_H
#define X52D_EVENT_HANDLER_H

#include <libx52/libx52.h>
#include <libx52/libx52io.h>

typedef void (*x52_event_handler_fn)(uint64_t changed,
                                     const libx52io_report *prev,
                                     const libx52io_report *curr);

int x52d_register_event_handler(uint64_t mask, x52_event_handler_fn fn);
int x52d_update_event_handler_mask(int handler, uint64_t new_mask);
void x52d_dispatch_event(libx52io_report *prev, libx52io_report *curr);
void x52d_dispatch_event_raw(uint64_t changed, libx52io_report *prev, libx52io_report *curr);

#endif // !defined X52D_EVENT_HANDLER_H
