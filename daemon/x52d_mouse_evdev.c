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
#include <pthread.h>
#include <unistd.h>

#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

static pthread_t mouse_thr;
static bool mouse_thr_enabled = false;

static void * x52_mouse_thr(void *param)
{
    PINELOG_INFO(_("Starting X52 virtual mouse driver thread"));
    for (;;) {
        usleep(mouse_delay);
        if (!mouse_enabled) {
            /* Mouse thread is disabled, check again next time */
            continue;
        }
    }

    return NULL;
}

static void x52d_mouse_thr_init(void)
{
    int rc;

    PINELOG_TRACE("Initializing virtual mouse driver");
    rc = pthread_create(&mouse_thr, NULL, x52_mouse_thr, NULL);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d initializing mouse thread: %s"),
                      rc, strerror(rc));
    }
}

static void x52d_mouse_thr_exit(void)
{
    PINELOG_INFO(_("Shutting down X52 virtual mouse driver thread"));
    pthread_cancel(mouse_thr);
}

void x52d_mouse_evdev_thread_control(bool enabled)
{
    if (enabled) {
        if (mouse_thr_enabled) {
            PINELOG_TRACE("Ignoring re-enable mouse thread");
            return;
        } else {
            x52d_mouse_thr_init();
        }
    } else {
        if (!mouse_thr_enabled) {
            PINELOG_TRACE("Ignoring re-disable mouse thread");
            return;
        } else {
            x52d_mouse_thr_exit();
        }
    }
    mouse_thr_enabled = enabled;
}
