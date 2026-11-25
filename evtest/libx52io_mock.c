/*
 * Saitek X52 Pro MFD & LED driver - Mock libx52io for evtest unit testing
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <libx52/libx52io.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static char evtest_mock_ctx_storage;

int libx52io_init(libx52io_context **ctx)
{
    if (!ctx) {
        return LIBX52IO_ERROR_INVALID;
    }
    *ctx = (libx52io_context *)&evtest_mock_ctx_storage;
    return LIBX52IO_SUCCESS;
}

void libx52io_exit(libx52io_context *ctx)
{
    (void)ctx;
}

int libx52io_open(libx52io_context *ctx)
{
    (void)ctx;
    return LIBX52IO_SUCCESS;
}

int libx52io_close(libx52io_context *ctx)
{
    (void)ctx;
    return LIBX52IO_SUCCESS;
}

const char *libx52io_strerror(libx52io_error_code code)
{
    (void)code;
    return "";
}

uint16_t libx52io_get_vendor_id(libx52io_context *ctx)
{
    (void)ctx;
    return 0x06a3;
}

uint16_t libx52io_get_product_id(libx52io_context *ctx)
{
    (void)ctx;
    return 0x0762;
}

uint16_t libx52io_get_device_version(libx52io_context *ctx)
{
    (void)ctx;
    return 0x0110;
}

const char *libx52io_get_manufacturer_string(libx52io_context *ctx)
{
    (void)ctx;
    return "Saitek";
}

const char *libx52io_get_product_string(libx52io_context *ctx)
{
    (void)ctx;
    return "X52 Pro Flight Control System";
}

const char *libx52io_get_serial_number_string(libx52io_context *ctx)
{
    (void)ctx;
    return "TEST-SERIAL";
}

int libx52io_get_axis_range(libx52io_context *ctx, libx52io_axis axis, int32_t *min, int32_t *max)
{
    (void)ctx;
    if (!min || !max) {
        return LIBX52IO_ERROR_INVALID;
    }
    if (axis < LIBX52IO_AXIS_X || axis >= LIBX52IO_AXIS_MAX) {
        return LIBX52IO_ERROR_INVALID;
    }

    *min = 0;
    switch (axis) {
    case LIBX52IO_AXIS_X:
    case LIBX52IO_AXIS_Y:
    case LIBX52IO_AXIS_RZ:
        *max = 1023;
        break;
    case LIBX52IO_AXIS_Z:
    case LIBX52IO_AXIS_RX:
    case LIBX52IO_AXIS_RY:
    case LIBX52IO_AXIS_SLIDER:
        *max = 255;
        break;
    case LIBX52IO_AXIS_THUMBX:
    case LIBX52IO_AXIS_THUMBY:
        *max = 15;
        break;
    case LIBX52IO_AXIS_HATX:
    case LIBX52IO_AXIS_HATY:
        *min = -1;
        *max = 1;
        break;
    default:
        *max = 0;
        break;
    }

    return LIBX52IO_SUCCESS;
}

int libx52io_read_timeout(libx52io_context *ctx, libx52io_report *report, int timeout)
{
    (void)ctx;
    (void)timeout;
    static int read_count = 0;

    if (!report) {
        return LIBX52IO_ERROR_INVALID;
    }

    memset(report, 0, sizeof(*report));

    switch (read_count++) {
    case 0:
        /* First read: a button press */
        report->button[LIBX52IO_BTN_TRIGGER] = true;
        return LIBX52IO_SUCCESS;

    case 1:
        /* Second read: axis movement */
        report->axis[LIBX52IO_AXIS_X] = 512;
        return LIBX52IO_SUCCESS;

    case 2:
        /* Third read: simulate SIGINT to terminate main's loop */
        raise(SIGINT);
        return LIBX52IO_ERROR_TIMEOUT;

    default:
        /* Subsequent calls if any: signal an error / abort */
        return LIBX52IO_ERROR_IO;
    }
}
