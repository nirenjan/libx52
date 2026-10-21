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

uint64_t libx52io_compare_reports(const libx52io_report *prev,
                                  const libx52io_report *curr)
{
    uint64_t status = 0;

    if (prev == NULL || curr == NULL) {
        // Treat either input being NULL as no change.
        return 0;
    }

#define CHECK_AXIS(axs) do { \
    if (prev->axis[LIBX52IO_AXIS_ ## axs] != curr->axis[LIBX52IO_AXIS_ ## axs]) { \
        status |= LIBX52IO_REPORT_AXIS_ ## axs; \
    } \
} while(0)

    CHECK_AXIS(X);
    CHECK_AXIS(Y);
    CHECK_AXIS(RZ);
    CHECK_AXIS(Z);
    CHECK_AXIS(RX);
    CHECK_AXIS(RY);
    CHECK_AXIS(SLIDER);
    CHECK_AXIS(THUMBX);
    CHECK_AXIS(THUMBY);
    CHECK_AXIS(HATX);
    CHECK_AXIS(HATY);
#undef CHECK_AXIS

#define CHECK_BUTTON(btn) do { \
    if (prev->button[LIBX52IO_BTN_ ## btn] != curr->button[LIBX52IO_BTN_ ## btn]) { \
        status |= LIBX52IO_REPORT_BTN_ ## btn; \
    } \
} while(0)

    CHECK_BUTTON(TRIGGER);
    CHECK_BUTTON(TRIGGER_2);
    CHECK_BUTTON(FIRE);
    CHECK_BUTTON(PINKY);
    CHECK_BUTTON(A);
    CHECK_BUTTON(B);
    CHECK_BUTTON(C);
    CHECK_BUTTON(D);
    CHECK_BUTTON(E);
    CHECK_BUTTON(T1_UP);
    CHECK_BUTTON(T1_DN);
    CHECK_BUTTON(T2_UP);
    CHECK_BUTTON(T2_DN);
    CHECK_BUTTON(T3_UP);
    CHECK_BUTTON(T3_DN);
    CHECK_BUTTON(POV_1_N);
    CHECK_BUTTON(POV_1_E);
    CHECK_BUTTON(POV_1_S);
    CHECK_BUTTON(POV_1_W);
    CHECK_BUTTON(POV_2_N);
    CHECK_BUTTON(POV_2_E);
    CHECK_BUTTON(POV_2_S);
    CHECK_BUTTON(POV_2_W);
    CHECK_BUTTON(CLUTCH);
    CHECK_BUTTON(MOUSE_PRIMARY);
    CHECK_BUTTON(MOUSE_SECONDARY);
    CHECK_BUTTON(MOUSE_SCROLL_UP);
    CHECK_BUTTON(MOUSE_SCROLL_DN);
    CHECK_BUTTON(FUNCTION);
    CHECK_BUTTON(START_STOP);
    CHECK_BUTTON(RESET);
    CHECK_BUTTON(PG_UP);
    CHECK_BUTTON(PG_DN);
    CHECK_BUTTON(UP);
    CHECK_BUTTON(DN);
    CHECK_BUTTON(SELECT);
    CHECK_BUTTON(MODE_1);
    CHECK_BUTTON(MODE_2);
    CHECK_BUTTON(MODE_3);
#undef CHECK_BUTTON

    if (prev->mode != curr->mode) {
        status |= LIBX52IO_REPORT_MODE;
    }

    if (prev->hat != curr->hat) {
        status |= LIBX52IO_REPORT_HAT;
    }

    return status;
}
