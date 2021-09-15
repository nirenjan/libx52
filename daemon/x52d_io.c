/*
 * Saitek X52 Pro MFD & LED driver - I/O driver
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "x52d_const.h"
#include "x52d_config.h"
#include "x52d_io.h"
#include "x52d_mouse.h"
#include "libx52io.h"
#include "pinelog.h"

static libx52io_context *io_ctx;

static pthread_t io_thr;

static void process_report(libx52io_report *report, libx52io_report *prev)
{
    // TODO: Process changes
    x52d_mouse_report_event(report);
    memcpy(prev, report, sizeof(*prev));
}

static void *x52_io_thr(void *param)
{
    int rc;
    libx52io_report report;
    libx52io_report prev_report;

    #define IO_READ_TIMEOUT 50 /* milliseconds */
    #define IO_ACQ_TIMEOUT 5 /* seconds */
    PINELOG_INFO(_("Starting X52 I/O thread"));

    // Reset the previous report, so that process_report  can handle changes.
    memset(&prev_report, 0, sizeof(prev_report));

    for (;;) {
        rc = libx52io_read_timeout(io_ctx, &report, IO_READ_TIMEOUT);
        switch (rc) {
        case LIBX52IO_SUCCESS:
            // Found a report
            process_report(&report, &prev_report);
            break;

        case LIBX52IO_ERROR_TIMEOUT:
            // No report received within the timeout
            break;

        case LIBX52IO_ERROR_NO_DEVICE:
            PINELOG_TRACE("Device disconnected, trying to connect");
            rc = libx52io_open(io_ctx);
            if (rc != LIBX52IO_SUCCESS) {
                if (rc != LIBX52IO_ERROR_NO_DEVICE) {
                    PINELOG_ERROR(_("Error %d opening X52 I/O device: %s"),
                                  rc, libx52io_strerror(rc));
                } else {
                    PINELOG_TRACE("No compatible X52 I/O device found. Sleeping %d seconds before trying again.",
                                  IO_ACQ_TIMEOUT);
                }
                sleep(IO_ACQ_TIMEOUT);
            }
            break;

        default:
            PINELOG_ERROR(_("Error %d reading from X52 I/O device: %s"),
                          rc, libx52io_strerror(rc));

            /*
             * Possibly disconnected, better to force disconnect now, and try
             * to reconnect later
             */
            libx52io_close(io_ctx);

            /* Report a NULL report to reset the mouse to default state */
            x52d_mouse_report_event(NULL);
            break;
        }
    }
    #undef IO_READ_TIMEOUT
    #undef IO_ACQ_TIMEOUT

    return NULL;
}

void x52d_io_init(void)
{
    int rc;

    PINELOG_TRACE("Initializing I/O driver");
    rc = libx52io_init(&io_ctx);
    if (rc != LIBX52IO_SUCCESS) {
        PINELOG_FATAL(_("Error %d initializing X52 I/O library: %s"),
                      rc, libx52io_strerror(rc));
    }

    rc = pthread_create(&io_thr, NULL, x52_io_thr, NULL);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d initializing I/O driver thread: %s"),
                      rc, strerror(rc));
    }
}

void x52d_io_exit(void)
{
    PINELOG_INFO(_("Shutting down X52 I/O driver thread"));
    pthread_cancel(io_thr);

    libx52io_exit(io_ctx);
}
