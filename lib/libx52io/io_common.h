/*
 * Saitek X52 IO driver - common definitions
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef IO_COMMON_H
#define IO_COMMON_H

#include <stdint.h>
#include "libx52io.h"
#include "hidapi.h"

// Function handler for parsing reports
typedef int (*x52_parse_report)(unsigned char *data, int length, libx52io_report *report);

struct libx52io_context {
    hid_device *handle;

    int32_t axis_min[LIBX52IO_AXIS_MAX];
    int32_t axis_max[LIBX52IO_AXIS_MAX];

    int16_t pid;
    x52_parse_report parser;
};

void _x52io_set_axis_range(libx52io_context *ctx);
void _x52io_set_report_parser(libx52io_context *ctx);
int _x52io_parse_report(libx52io_context *ctx, libx52io_report *report,
                        unsigned char *data, int length);

#endif // !defined IO_COMMON_H
