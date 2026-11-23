/*
 * Saitek X52 Pro MFD & LED driver - Event test formatting and dispatch
 *
 * Copyright (C) 2012-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include <libx52/libx52io.h>
#include "ev_denoise.h"
#include "ev_dispatch.h"
#include "gettext.h"

#define _(x) gettext(x)

int evtest_init_denoise_masks(libx52io_context *ctx,
                              int32_t denoise_mask[LIBX52IO_AXIS_MAX],
                              bool denoise)
{
    for (int i = 0; i < LIBX52IO_AXIS_MAX; i++) {
        denoise_mask[i] = 0;
    }

    if (!denoise) {
        return LIBX52IO_SUCCESS;
    }

    for (int i = LIBX52IO_AXIS_X; i < LIBX52IO_AXIS_MAX; i++) {
        int32_t min, max;
        int rc = libx52io_get_axis_range(ctx, i, &min, &max);
        if (rc != LIBX52IO_SUCCESS) {
            return rc;
        }

        /*
         * Denoising algorithm ignores the last few bits of the axis,
         * and is based on the maximum value of the axis. The mask is
         * ~(max >> 6) which will do nothing for the axis with a small
         * range, but reduce the noise on those with a larger range.
         */
        denoise_mask[i] = evtest_axis_denoise_mask(max);
    }

    return LIBX52IO_SUCCESS;
}

void evtest_print_device_info(FILE *out, libx52io_context *ctx)
{
    fprintf(out, _("Device ID: vendor 0x%04x product 0x%04x version 0x%04x\n"),
            libx52io_get_vendor_id(ctx),
            libx52io_get_product_id(ctx),
            libx52io_get_device_version(ctx));
    fprintf(out, _("Device name: \"%s %s\"\n"),
            libx52io_get_manufacturer_string(ctx),
            libx52io_get_product_string(ctx));
    fprintf(out, _("Serial number: \"%s\"\n"), libx52io_get_serial_number_string(ctx));
    fputs(_("Testing (interrupt to exit)\n\n"), out);
}

bool evtest_format_events(FILE *out,
                          const struct timeval *tv,
                          const libx52io_report *last,
                          const libx52io_report *curr,
                          const int32_t denoise_mask[LIBX52IO_AXIS_MAX],
                          bool denoise)
{
    bool printed = false;

    for (int axis = 0; axis < LIBX52IO_AXIS_MAX; axis++) {
        if (evtest_axis_changed(last->axis[axis], curr->axis[axis],
                                denoise_mask[axis], denoise)) {
            fprintf(out, _("Event @ %ld.%06ld: %s, value %d\n"),
                    (long int)tv->tv_sec, (long int)tv->tv_usec,
                    libx52io_axis_to_str(axis), curr->axis[axis]);
            printed = true;
        }
    }

    for (int btn = 0; btn < LIBX52IO_BUTTON_MAX; btn++) {
        if (last->button[btn] != curr->button[btn]) {
            fprintf(out, _("Event @ %ld.%06ld: %s, value %d\n"),
                    (long int)tv->tv_sec, (long int)tv->tv_usec,
                    libx52io_button_to_str(btn), curr->button[btn]);
            printed = true;
        }
    }

    if (printed) {
        fputc('\n', out);
    }

    return printed;
}
