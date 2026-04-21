/*
 * Saitek X52 Pro MFD & LED driver - Event test axis denoise helpers
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef EVTEST_EV_DENOISE_H
#define EVTEST_EV_DENOISE_H

#include <stdbool.h>
#include <stdint.h>

int32_t evtest_axis_denoise_mask(int32_t axis_max);

bool evtest_axis_changed(int32_t last, int32_t curr, int32_t mask, bool denoise_enabled);

#endif /* EVTEST_EV_DENOISE_H */
