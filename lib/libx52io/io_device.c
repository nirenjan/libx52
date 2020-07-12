/*
 * Saitek X52 IO driver - device information
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "io_common.h"

static char * _save_string(wchar_t *wcs)
{
    int n;
    char *out_str = NULL;

    if (wcs == NULL) {
        return NULL;
    }

    /*
     * Get the number of bytes needed to save the wide character string as
     * a multibyte string
     */
    n = wcstombs(NULL, wcs, 0);
    if (n <= 0) {
        /* Two possibilities here:
         * n == -1; A wide character that couldn't be converted was found.
         * n == 0; No wide characters were in the input string.
         *
         * In the latter case, we don't have anything to convert, while in
         * the former case, we don't know how long the string is to convert.
         *
         * Return NULL in both cases.
         */
        return NULL;
    }

    n++;
    out_str = calloc(n, sizeof(*out_str));
    if (out_str != NULL) {
        wcstombs(out_str, wcs, n);
    }

    return out_str;
}

static void _free_string(char **str)
{
    if (*str != NULL) {
        free(*str);
        *str = NULL;
    }
}

void _x52io_save_device_info(libx52io_context *ctx, struct hid_device_info *dev)
{
    ctx->vid = dev->vendor_id;
    ctx->pid = dev->product_id;
    ctx->version = dev->release_number;

    ctx->manufacturer = _save_string(dev->manufacturer_string);
    ctx->product = _save_string(dev->product_string);
    ctx->serial_number = _save_string(dev->serial_number);

    _x52io_set_axis_range(ctx);
    _x52io_set_report_parser(ctx);
}

void _x52io_release_device_info(libx52io_context *ctx)
{
    ctx->vid = 0;
    ctx->pid = 0;
    ctx->version = 0;

    _free_string(&(ctx->manufacturer));
    _free_string(&(ctx->product));
    _free_string(&(ctx->serial_number));

    memset(ctx->axis_min, 0, sizeof(ctx->axis_min));
    memset(ctx->axis_max, 0, sizeof(ctx->axis_max));
    ctx->parser = NULL;
    ctx->handle = NULL;
}

uint16_t libx52io_get_vendor_id(libx52io_context *ctx)
{
    return (ctx ? ctx->vid : 0);
}

uint16_t libx52io_get_product_id(libx52io_context *ctx)
{
    return (ctx ? ctx->pid : 0);
}

uint16_t libx52io_get_device_version(libx52io_context *ctx)
{
    return (ctx ? ctx->version : 0);
}

const char * libx52io_get_manufacturer_string(libx52io_context *ctx)
{
    return (ctx ? ctx->manufacturer : NULL);
}

const char * libx52io_get_product_string(libx52io_context *ctx)
{
    return (ctx ? ctx->product : NULL);
}

const char * libx52io_get_serial_number_string(libx52io_context *ctx)
{
    return (ctx ? ctx->serial_number : NULL);
}

