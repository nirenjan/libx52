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

#include "libevdev/libevdev.h"
#include "libevdev/libevdev-uinput.h"
#include "libx52io.h"

#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

static pthread_t mouse_thr;
static bool mouse_thr_enabled = false;

static struct libevdev_uinput *mouse_uidev;
static bool mouse_uidev_created = false;

static volatile libx52io_report old_report;
static volatile libx52io_report new_report;

static int report_button_change(int button, int index)
{
    int rc = 1;
    bool old_button = old_report.button[index];
    bool new_button = new_report.button[index];

    if (old_button != new_button) {
        rc = libevdev_uinput_write_event(mouse_uidev, EV_KEY, button,
                                    (int)new_button);
        if (rc != 0) {
            PINELOG_ERROR(_("Error writing mouse button event (button %d, state %d)"),
                          button, (int)new_button);
        }
    }

    return rc;
}

static int report_wheel(void)
{
    int rc = 1;
    int wheel = 0;
    bool scroll_up = new_report.button[LIBX52IO_BTN_MOUSE_SCROLL_UP];
    bool scroll_dn = new_report.button[LIBX52IO_BTN_MOUSE_SCROLL_DN];

    if (scroll_up) {
        // Scroll up event
        wheel = 1;
    } else if (scroll_dn) {
        // Scroll down event
        wheel = -1;
    }

    if (wheel != 0) {
        rc = libevdev_uinput_write_event(mouse_uidev, EV_REL, REL_WHEEL, wheel);
        if (rc != 0) {
            PINELOG_ERROR(_("Error writing mouse wheel event %d"), wheel);
        }
    }

    return rc;
}

static int report_axis(int axis, int index)
{
    int rc = 1;

    int axis_val = new_report.axis[index];

    /*
     * Axis value ranges from 0 to 15, with the default midpoint at 8.
     * We need to translate this to a range of -7 to +7. Since the midpoint
     * is slightly off-center, we will shift the values left, and subtract
     * 15, effectively, giving us a range of -15 to +15. Shifting right again
     * will reduce the range to -7 to +7, and effectively ignore the reported
     * values of 7 and 8.
     */
    axis_val = ((axis_val << 1) - 15) >> 1;

    /*
     * Factor in the multiplicative factor for the axis. This deliberately
     * uses integer division, since the uinput event only accepts integers.
     * For the speed purposes, this should be good enough.
     */
    axis_val = (axis_val * mouse_mult) / MOUSE_MULT_FACTOR;

    if (axis_val) {
        rc = libevdev_uinput_write_event(mouse_uidev, EV_REL, axis, axis_val);
        if (rc != 0) {
            PINELOG_ERROR(_("Error writing mouse axis event (axis %d, value %d)"),
                          axis, axis_val);
        }
    }

    return rc;
}

static void report_sync(void)
{
    int rc;
    rc = libevdev_uinput_write_event(mouse_uidev, EV_SYN, SYN_REPORT, 0);
    if (rc != 0) {
        PINELOG_ERROR(_("Error writing mouse sync event"));
    } else {
        memcpy((void *)&old_report, (void *)&new_report, sizeof(old_report));
    }
}

static void reset_reports(void)
{
    memset((void *)&old_report, 0, sizeof(old_report));
    /* Set the default thumbstick values to the mid-point */
    old_report.axis[LIBX52IO_AXIS_THUMBX] = 8;
    old_report.axis[LIBX52IO_AXIS_THUMBY] = 8;
    memcpy((void *)&new_report, (void *)&old_report, sizeof(new_report));
}

static void * x52_mouse_thr(void *param)
{
    bool state_changed;

    PINELOG_INFO(_("Starting X52 virtual mouse driver thread"));
    for (;;) {
        state_changed = false;
        state_changed |= (0 == report_axis(REL_X, LIBX52IO_AXIS_THUMBX));
        state_changed |= (0 == report_axis(REL_Y, LIBX52IO_AXIS_THUMBY));

        if (state_changed) {
            report_sync();
        }

        usleep(mouse_delay);
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
    if (!mouse_uidev_created) {
        PINELOG_INFO(_("Virtual mouse not created. Ignoring thread state change"));
        return;
    }

    if (enabled) {
        if (mouse_thr_enabled) {
            PINELOG_TRACE("Ignoring re-enable mouse thread");
            return;
        } else {
            reset_reports();
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

void x52d_mouse_report_event(libx52io_report *report)
{
    bool state_changed;
    if (report) {
        memcpy((void *)&new_report, report, sizeof(new_report));

        state_changed = false;
        state_changed |= (0 == report_button_change(BTN_LEFT, LIBX52IO_BTN_MOUSE_PRIMARY));
        state_changed |= (0 == report_button_change(BTN_RIGHT, LIBX52IO_BTN_MOUSE_SECONDARY));
        state_changed |= (0 == report_wheel());

        if (state_changed) {
            report_sync();
        }
    } else {
        reset_reports();
    }
}

void x52d_mouse_evdev_init(void)
{
    int rc;
    struct libevdev *dev;

    /* Create a new mouse device */
    dev = libevdev_new();
    libevdev_set_name(dev, "X52 virtual mouse");
    libevdev_enable_event_type(dev, EV_REL);
    libevdev_enable_event_code(dev, EV_REL, REL_X, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_Y, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_WHEEL, NULL);
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT, NULL);

    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
                                            &mouse_uidev);
    if (rc != 0) {
        PINELOG_ERROR(_("Error %d creating X52 virtual mouse: %s"),
                      -rc, strerror(-rc));
    } else {
        mouse_uidev_created = true;
    }
}

void x52d_mouse_evdev_exit(void)
{
    x52d_mouse_evdev_thread_control(false);
    mouse_uidev_created = false;
    libevdev_uinput_destroy(mouse_uidev);
}
