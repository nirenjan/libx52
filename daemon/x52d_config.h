/*
 * Saitek X52 Pro MFD & LED driver - Configuration parser header
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_CONFIG_H
#define X52D_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "libx52.h"

/**
 * @brief Configuration structure
 *
 * Keep this in sync with the sample configuration
 */
struct x52d_config {
    bool    clock_enabled;
    bool    primary_clock_local;
    char    clock_2_tz[NAME_MAX];
    char    clock_3_tz[NAME_MAX];

    // Since we don't have a _MAX identifier for libx52_led_id, hardcode
    // the length in the following declaration.
    libx52_led_state    leds[21];

    uint16_t brightness[2];

    bool clutch_enabled;
    bool clutch_latched;

    char profiles_dir[NAME_MAX];
};

int x52d_config_set_defaults(struct x52d_config *cfg);

int x52d_config_load_file(struct x52d_config *cfg, const char *cfg_file);

int x52d_config_save_override(const char *override_str);

int x52d_config_apply_overrides(struct x52d_config *cfg);

void x52d_config_clear_overrides(void);

void x52d_config_load(const char *cfg_file);
void x52d_config_apply(void);

#endif // !defined X52D_CONFIG_H
