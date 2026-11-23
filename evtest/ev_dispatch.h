/*
 * Saitek X52 Pro MFD & LED driver - Event test formatting and dispatch
 *
 * Copyright (C) 2012-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef EVTEST_EV_DISPATCH_H
#define EVTEST_EV_DISPATCH_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <libx52/libx52io.h>

/**
 * @brief Initialize axis denoise masks based on axis ranges.
 *
 * @param[in]  ctx           Device context
 * @param[out] denoise_mask  Array of size LIBX52IO_AXIS_MAX to populate
 * @param[in]  denoise       Whether denoising is enabled
 * @return 0 on success, or libx52io error code on failure
 */
int evtest_init_denoise_masks(libx52io_context *ctx,
                              int32_t denoise_mask[LIBX52IO_AXIS_MAX],
                              bool denoise);

/**
 * @brief Print connected device header info (ID, name, serial number).
 *
 * @param[in] out  Output stream (e.g. stdout)
 * @param[in] ctx  Device context
 */
void evtest_print_device_info(FILE *out, libx52io_context *ctx);

/**
 * @brief Compare current report against last report and format changed events.
 *
 * Checks all axes and buttons for changes (taking denoising into account for axes),
 * and prints timestamped event lines followed by an empty trailing line if any
 * event was printed.
 *
 * @param[in] out           Output stream (e.g. stdout)
 * @param[in] tv            Timestamp of the event
 * @param[in] last          Previous report
 * @param[in] curr          Current report
 * @param[in] denoise_mask  Denoise mask array
 * @param[in] denoise       Whether denoising is enabled
 * @return true if one or more event lines were printed, false otherwise
 */
bool evtest_format_events(FILE *out,
                          const struct timeval *tv,
                          const libx52io_report *last,
                          const libx52io_report *curr,
                          const int32_t denoise_mask[LIBX52IO_AXIS_MAX],
                          bool denoise);

#endif /* EVTEST_EV_DISPATCH_H */
