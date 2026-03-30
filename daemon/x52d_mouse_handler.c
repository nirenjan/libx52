/*
 * Saitek X52 Pro MFD & LED driver - Mouse driver
 *
 * Copyright (C) 2021-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "libx52io.h"
#include "vkm.h"

#define PINELOG_MODULE X52D_MOD_MOUSE
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_mouse.h"

static pthread_t mouse_thr;
static bool mouse_thr_enabled = false;

static vkm_context *mouse_context;

static volatile libx52io_report old_report;
static volatile libx52io_report new_report;

static int report_button_change(vkm_mouse_button button, int index)
{
    vkm_result rc = VKM_ERROR_NO_CHANGE;
    bool old_button = old_report.button[index];
    bool new_button = new_report.button[index];
    vkm_button_state state;

    if (old_button != new_button) {
        state = new_button ? VKM_BUTTON_PRESSED : VKM_BUTTON_RELEASED;
        rc = vkm_mouse_click(mouse_context, button, state);
        if (rc != VKM_SUCCESS && rc != VKM_ERROR_NO_CHANGE) {
            PINELOG_ERROR(_("Error %d writing mouse button event (button %d, state %d)"),
                          rc, button, (int)new_button);
        }
    }

    return (rc == VKM_SUCCESS);
}

static int report_wheel(void)
{
    vkm_result rc = VKM_ERROR_NO_CHANGE;
    int wheel = 0;
    bool scroll_up = new_report.button[LIBX52IO_BTN_MOUSE_SCROLL_UP];
    bool scroll_dn = new_report.button[LIBX52IO_BTN_MOUSE_SCROLL_DN];
    bool old_scroll_up = old_report.button[LIBX52IO_BTN_MOUSE_SCROLL_UP];
    bool old_scroll_dn = old_report.button[LIBX52IO_BTN_MOUSE_SCROLL_DN];
    vkm_mouse_scroll_direction dir;

    /*
     * Handle multiple scroll button presses in sequence. This happens if a
     * hardware axis is very noisy and the firmware sends a sequence of reports
     * with button down, even though this is technically a momentary button.
     */
    scroll_up = (scroll_up ^ old_scroll_up) & scroll_up;
    scroll_dn = (scroll_dn ^ old_scroll_dn) & scroll_dn;

    if (scroll_up) {
        // Scroll up event
        wheel = 1 * mouse_scroll_dir;
    } else if (scroll_dn) {
        // Scroll down event
        wheel = -1 * mouse_scroll_dir;
    }

    if (wheel != 0) {
        dir = (wheel == 1) ? VKM_MOUSE_SCROLL_UP : VKM_MOUSE_SCROLL_DOWN;
        rc = vkm_mouse_scroll(mouse_context, dir);
        if (rc != VKM_SUCCESS) {
            PINELOG_ERROR(_("Error writing mouse wheel event %d"), dir);
        }
    }

    return (rc == VKM_SUCCESS);
}

static int get_axis_val(int index)
{
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

    return axis_val;
}

static int report_axis(void)
{
    vkm_result rc;
    int dx = get_axis_val(LIBX52IO_AXIS_THUMBX);
    int dy = get_axis_val(LIBX52IO_AXIS_THUMBY);

    rc = vkm_mouse_move(mouse_context, dx, dy);
    if (rc != VKM_SUCCESS && rc != VKM_ERROR_NO_CHANGE) {
        PINELOG_ERROR(_("Error %d writing mouse axis event (dx %d, dy %d)"),
                rc, dx, dy);
    }

    return (rc == VKM_SUCCESS);
}

static void report_sync(void)
{
    vkm_result rc;
    rc = vkm_sync(mouse_context);
    if (rc != VKM_SUCCESS) {
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
    (void)param;

    PINELOG_INFO(_("Starting X52 virtual mouse driver thread"));
    for (;;) {
        if (report_axis()) {
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

void x52d_mouse_thread_control(bool enabled)
{
    if (!vkm_is_ready(mouse_context)) {
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

        if (!vkm_is_ready(mouse_context) || !mouse_thr_enabled) {
            return;
        }

        state_changed = false;
        state_changed |= (0 == report_button_change(VKM_MOUSE_BTN_LEFT, LIBX52IO_BTN_MOUSE_PRIMARY));
        state_changed |= (0 == report_button_change(VKM_MOUSE_BTN_RIGHT, LIBX52IO_BTN_MOUSE_SECONDARY));
        state_changed |= (0 == report_wheel());

        if (state_changed) {
            report_sync();
        }
    } else {
        reset_reports();
    }
}

void x52d_mouse_handler_init(void)
{
    vkm_result rc;

    rc = vkm_init(&mouse_context);
    if (rc != VKM_SUCCESS) {
        PINELOG_ERROR(_("Error %d creating X52 virtual mouse"), rc);
        return;
    }

    vkm_set_option(mouse_context, VKM_OPT_DEVICE_NAME, "X52 virtual mouse");

    rc = vkm_start(mouse_context);
    if (rc != VKM_SUCCESS) {
        PINELOG_ERROR(_("Error %d creating X52 virtual mouse"), rc);
    }
}

void x52d_mouse_handler_exit(void)
{
    x52d_mouse_thread_control(false);
    vkm_exit(mouse_context);
    mouse_context = NULL;
}
