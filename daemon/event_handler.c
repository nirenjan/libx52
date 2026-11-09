/*
 * Saitek X52 Pro MFD & LED driver - Event handler
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdatomic.h>
#include <errno.h>

#include "event_handler.h"

typedef struct {
    _Atomic x52_event_handler_fn fn;
    _Atomic uint64_t mask;
} x52_event_handler;

#define MAX_EVENT_HANDLERS 8

static x52_event_handler handlers[MAX_EVENT_HANDLERS];
static atomic_int num_handlers = 0;

int x52d_register_event_handler(uint64_t mask, x52_event_handler_fn fn)
{
    int ticket;

    if (fn == NULL) {
        // Don't bother registering a NULL callback
        errno = EINVAL;
        return -1;
    }

    ticket = atomic_fetch_add(&num_handlers, 1);
    if (ticket >= MAX_EVENT_HANDLERS) {
        // Already hit the maximum tickets
        errno = ENOSPC;
        return -1;
    }

    atomic_store(&handlers[ticket].mask, mask);
    atomic_store(&handlers[ticket].fn, fn);

    return ticket;
}

int x52d_update_event_handler_mask(int handler, uint64_t new_mask)
{
    if (handler < 0 || handler >= MAX_EVENT_HANDLERS) {
        errno = ERANGE;
        return -1;
    }

    if (atomic_load(&handlers[handler].fn) == NULL) {
        errno = EINVAL;
        return -1;
    }

    atomic_store(&handlers[handler].mask, new_mask);
    return 0;
}

void x52d_dispatch_event(libx52io_report *prev, libx52io_report *curr)
{
    int current_limit = atomic_load(&num_handlers);
    if (current_limit >= MAX_EVENT_HANDLERS) {
        current_limit = MAX_EVENT_HANDLERS;
    }

    uint64_t changed = libx52io_compare_reports(prev, curr);


    for (int i = 0; i < current_limit; i++) {
        x52_event_handler_fn callback = atomic_load(&handlers[i].fn);
        if (callback == NULL) {
            continue;
        }

        uint64_t mask = atomic_load(&handlers[i].mask) & changed;
        if (mask != 0) {
            callback(mask, prev, curr);
        }
    }
}
