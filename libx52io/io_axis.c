/*
 * Saitek X52 IO driver - axis ranges
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <string.h>
#include "io_common.h"
#include "usb-ids.h"

static const int32_t axis_min[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1
};

static const int32_t x52_axis_max[] = {
    2047, 2047, 1023, 255, 255, 255, 255, 15, 15, 1, 1
};

static const int32_t x52pro_axis_max[] = {
    1023, 1023, 1023, 255, 255, 255, 255, 15, 15, 1, 1
};

void _x52io_set_axis_range(libx52io_context *ctx)
{
    switch (ctx->pid) {
    case X52_PROD_X52_1:
    case X52_PROD_X52_2:
        memcpy(ctx->axis_min, axis_min, sizeof(ctx->axis_min));
        memcpy(ctx->axis_max, x52_axis_max, sizeof(ctx->axis_max));
        break;

    case X52_PROD_X52PRO:
        memcpy(ctx->axis_min, axis_min, sizeof(ctx->axis_min));
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

    if (!(axis >= LIBX52IO_AXIS_X && axis < LIBX52IO_AXIS_MAX)) {
        return LIBX52IO_ERROR_INVALID;
    }

    if (ctx->handle == NULL) {
        return LIBX52IO_ERROR_NO_DEVICE;
    }

    *min = ctx->axis_min[axis];
    *max = ctx->axis_max[axis];

    return LIBX52IO_SUCCESS;
}
