/*
 * Saitek X52 IO driver - axis ranges
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <string.h>
#include "io_common.h"

static const int32_t x52_axis_max[] = {
    2047, 2047, 1023, 255, 255, 255, 255, 15, 15
};

static const int32_t x52pro_axis_max[] = {
    1023, 1023, 1023, 255, 255, 255, 255, 15, 15
};

void _x52io_set_axis_range(libx52io_context *ctx)
{
    switch (ctx->pid) {
    case 0x0255:
    case 0x075c:
        memcpy(ctx->axis_max, x52_axis_max, sizeof(ctx->axis_max));
        break;

    case 0x0762:
        memcpy(ctx->axis_max, x52pro_axis_max, sizeof(ctx->axis_max));
        break;

    default:
        break;
    }
}

int libx52io_get_axis_range(libx52io_context *ctx,
                            libx52io_axis axis,
                            int32_t *min,
                            int32_t *max)
{
    if (ctx == NULL || min == NULL || max == NULL) {
        return LIBX52IO_ERROR_INVALID;
    }

    if (axis < 0 || axis >= LIBX52IO_AXIS_MAX) {
        return LIBX52IO_ERROR_INVALID;
    }

    if (ctx->handle == NULL) {
        return LIBX52IO_ERROR_NO_DEVICE;
    }

    /*
     * All axis ranges start at 0, only the max value changes between
     * X52 and X52Pro
     */
    *min = 0;
    *max = ctx->axis_max[axis];

    return LIBX52IO_SUCCESS;
}
