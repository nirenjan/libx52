/*
 * Saitek X52 Pro MFD & LED driver - Event test utility
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "libx52io.h"
#include "gettext.h"

/*
Output Format
=============

Driver version is 0.0.0
Device ID: bus 0x3 vendor 0x06a3 product 0x0762 version 0x110
Device name: "Saitek X52 Pro Flight Control System"
Testing ... (interrupt to exit)

Event @ 1594431236.817842, ABS_X, value 512
Event @ 1594431236.817842, ABS_Y, value 511
Event @ 1594431236.817842, BTN_TRIGGER, value 1
Event @ 1594431236.817842, BTN_MODE_1, value 1

Event @ 1594431236.847810, BTN_MODE_1, value 0

Event @ 1594431236.877802, BTN_MODE_2, value 1

 */

static bool exit_loop = false;

static void signal_handler(int sig)
{
    exit_loop = true;
}

/* Denoising - reduce event noise due to adjacent values being reported */
static bool denoise = true;

/* For i18n */
#define _(x) gettext(x)
int main(int argc, char **argv)
{
    libx52io_context *ctx;
    libx52io_report last, curr;
    int32_t denoise_mask[LIBX52IO_AXIS_MAX] = { 0 };
    int rc;
    #define CHECK_RC() do { \
        if (rc != LIBX52IO_SUCCESS) { \
            fprintf(stderr, "%s\n", libx52io_strerror(rc)); \
            return rc; \
        } \
    } while(0)

    /* Initialize gettext */
    #if ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(LOCALEDIR);
    #endif

    memset(&last, 0, sizeof(last));
    memset(&curr, 0, sizeof(curr));

    /* Initialize libx52io */
    rc = libx52io_init(&ctx);
    CHECK_RC();

    /* Make sure that we have a device to connect to */
    rc = libx52io_open(ctx);
    CHECK_RC();

    /* Initialize denoising */
    if (denoise) {
        for (int i = LIBX52IO_AXIS_X; i < LIBX52IO_AXIS_MAX; i++) {
            int32_t min, max;
            rc = libx52io_get_axis_range(ctx, i, &min, &max);
            CHECK_RC();

            /*
             * Denoising algorithm ignores the last few bits of the axis,
             * and is based on the maximum value of the axis. The mask is
             * ~(max >> 6) which will do nothing for the axis with a small
             * range, but reduce the noise on those with a larger range.
             */
            denoise_mask[i] = ~(max >> 6);
        }
    }

    /* Set up the signal handler to terminate the loop on SIGTERM or SIGINT */
    exit_loop = false;
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    /* Print the driver version and the connected device */
    printf(_("Device ID: vendor 0x%04x product 0x%04x version 0x%04x\n"),
           libx52io_get_vendor_id(ctx),
           libx52io_get_product_id(ctx),
           libx52io_get_device_version(ctx));
    printf(_("Device name: \"%s %s\"\n"),
           libx52io_get_manufacturer_string(ctx),
           libx52io_get_product_string(ctx));
    printf(_("Serial number: \"%s\"\n"), libx52io_get_serial_number_string(ctx));
    puts(_("Testing (interrupt to exit)\n"));

    /* Wait until we get an event */
    while (!exit_loop) {
        struct timeval tv;
        bool printed = false;

        /* Wait for 1 second before timing out */
        rc = libx52io_read_timeout(ctx, &curr, 1000);
        if (rc == LIBX52IO_ERROR_TIMEOUT) {
            continue;
        } else if (rc != LIBX52IO_SUCCESS) {
            /* Some other error while reading. Abort the loop */
            break;
        }

        /*
         * Successful read, compare the current report against the previous
         * one and display the result
         */
        if (memcmp(&last, &curr, sizeof(curr)) == 0) {
            /* No change, ignore the output */
            continue;
        }

        /* Get the current timeval - we don't need a timezone */
        gettimeofday(&tv, NULL);
        for (int axis = 0; axis < LIBX52IO_AXIS_MAX; axis++) {
            if (last.axis[axis] != curr.axis[axis]) {
                /* Account for denoising */
                if (denoise) {
                    int32_t last_v = last.axis[axis] & denoise_mask[axis];
                    int32_t curr_v = curr.axis[axis] & denoise_mask[axis];

                    if (last_v == curr_v) {
                        /* Within the noise threshold */
                        continue;
                    }
                }

                printf(_("Event @ %ld.%06ld: %s, value %d\n"),
                    (long int)tv.tv_sec, (long int)tv.tv_usec,
                    libx52io_axis_to_str(axis), curr.axis[axis]);
                printed = true;
            }
        }
        for (int btn = 0; btn < LIBX52IO_BUTTON_MAX; btn++) {
            if (last.button[btn] != curr.button[btn]) {
                printf(_("Event @ %ld.%06ld: %s, value %d\n"),
                    (long int)tv.tv_sec, (long int)tv.tv_usec,
                    libx52io_button_to_str(btn), curr.button[btn]);
                printed = true;
            }
        }

        if (printed) {
            puts("");
        }

        memcpy(&last, &curr, sizeof(curr));
    }

    /* Close and exit the libx52io library */
    libx52io_close(ctx);
    libx52io_exit(ctx);
}
