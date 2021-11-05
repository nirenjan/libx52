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

    // Since we don't have a _MAX identifier for libx52_clock_id, use
    // the maximum clock ID + 1 as the length
    libx52_clock_format clock_format[LIBX52_CLOCK_3 + 1];
    libx52_date_format date_format;

    char    clock_2_tz[NAME_MAX];
    char    clock_3_tz[NAME_MAX];

    // Since we don't have a _MAX identifier for libx52_led_id, hardcode
    // the length in the following declaration.
    libx52_led_state    leds[21];

    int brightness[2];

    bool mouse_enabled;
    int mouse_speed;

    bool clutch_enabled;
    bool clutch_latched;

    char profiles_dir[NAME_MAX];
};

/* Callback functions for configuration */
// These functions are defined in the individual modules
void x52d_cfg_set_Clock_Enabled(bool param);
void x52d_cfg_set_Clock_PrimaryIsLocal(bool param);
void x52d_cfg_set_Clock_Secondary(char* param);
void x52d_cfg_set_Clock_Tertiary(char* param);
void x52d_cfg_set_Clock_FormatPrimary(libx52_clock_format param);
void x52d_cfg_set_Clock_FormatSecondary(libx52_clock_format param);
void x52d_cfg_set_Clock_FormatTertiary(libx52_clock_format param);
void x52d_cfg_set_Clock_DateFormat(libx52_date_format param);
void x52d_cfg_set_LED_Fire(libx52_led_state param);
void x52d_cfg_set_LED_Throttle(libx52_led_state param);
void x52d_cfg_set_LED_A(libx52_led_state param);
void x52d_cfg_set_LED_B(libx52_led_state param);
void x52d_cfg_set_LED_D(libx52_led_state param);
void x52d_cfg_set_LED_E(libx52_led_state param);
void x52d_cfg_set_LED_T1(libx52_led_state param);
void x52d_cfg_set_LED_T2(libx52_led_state param);
void x52d_cfg_set_LED_T3(libx52_led_state param);
void x52d_cfg_set_LED_POV(libx52_led_state param);
void x52d_cfg_set_LED_Clutch(libx52_led_state param);
void x52d_cfg_set_Brightness_MFD(uint16_t param);
void x52d_cfg_set_Brightness_LED(uint16_t param);
void x52d_cfg_set_Mouse_Enabled(bool param);
void x52d_cfg_set_Mouse_Speed(int param);
void x52d_cfg_set_Profiles_Directory(char* param);
void x52d_cfg_set_Profiles_ClutchEnabled(bool param);
void x52d_cfg_set_Profiles_ClutchLatched(bool param);

int x52d_config_process_kv(void *user, const char *section, const char *key, const char *value);

int x52d_config_set_defaults(struct x52d_config *cfg);

int x52d_config_load_file(struct x52d_config *cfg, const char *cfg_file);

int x52d_config_save_override(const char *override_str);

int x52d_config_apply_overrides(struct x52d_config *cfg);

void x52d_config_clear_overrides(void);

void x52d_config_load(const char *cfg_file);
void x52d_config_apply(void);

int x52d_config_save_file(struct x52d_config *cfg, const char *cfg_file);
void x52d_config_save(const char *cfg_file);

int x52d_config_set(const char *section, const char *key, const char *value);

#endif // !defined X52D_CONFIG_H
