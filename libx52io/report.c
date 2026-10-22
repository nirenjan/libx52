/*
 * Saitek X52 IO driver
 *
 * Copyright (C) 2012-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "usb-ids.h"
#include "gettext.h"

void libx52io_copy_report(libx52io_report *target, const libx52io_report *source)
{
    if (target == NULL || source == NULL) {
        return;
    }

    for (int ax = 0; ax < LIBX52IO_AXIS_MAX; ax++) {
        target->axis[ax] = source->axis[ax];
    }

    for (int btn = 0; btn < LIBX52IO_BUTTON_MAX; btn++) {
        target->button[btn] = source->button[btn];
    }

    target->mode = source->mode;
    target->hat = source->hat;
}

void libx52io_reset_report(libx52io_report *report)
{
    if (report == NULL) {
        return;
    }

    for (int ax = 0; ax < LIBX52IO_AXIS_MAX; ax++) {
        switch (ax) {
        case LIBX52IO_AXIS_THUMBX:
        case LIBX52IO_AXIS_THUMBY:
            report->axis[ax] = 8;
            break;

        default:
            report->axis[ax] = 0;
            break;
        }
    }

    for (int btn = 0; btn < LIBX52IO_BUTTON_MAX; btn++) {
        report->button[btn] = 0;
    }

    report->mode = 0;
    report->hat = 0;
}
