/*
 * Saitek X52 Pro MFD & LED driver - Event test utility
 *
 * Copyright (C) 2012-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <libx52/libx52io.h>
#include "ev_denoise.h"
#include "ev_dispatch.h"
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
    (void)sig;
    exit_loop = true;
}

/* Denoising - reduce event noise due to adjacent values being reported */
static bool denoise = true;

int main(void)
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
    rc = evtest_init_denoise_masks(ctx, denoise_mask, denoise);
    CHECK_RC();

    /* Set up the signal handler to terminate the loop on SIGTERM or SIGINT */
    exit_loop = false;
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    /* Print the connected device info */
    evtest_print_device_info(stdout, ctx);

    /* Wait until we get an event */
    while (!exit_loop) {
        struct timeval tv;

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
        if (libx52io_compare_reports(&last, &curr) == 0) {
            /* No change, ignore the output */
            continue;
        }

        /* Get the current timeval - we don't need a timezone */
        gettimeofday(&tv, NULL);
        evtest_format_events(stdout, &tv, &last, &curr, denoise_mask, denoise);

        memcpy(&last, &curr, sizeof(curr));
    }

    /* Close and exit the libx52io library */
    libx52io_close(ctx);
    libx52io_exit(ctx);
}
