/*
 * Saitek X52 Pro MFD & LED driver - Configuration parser header
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_CONFIG_H
#define X52D_CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <libx52/libx52.h>

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
    int mouse_sensitivity;
    bool mouse_reverse_scroll;
    bool mouse_isometric_mode;
    int mouse_curve_factor;
    int mouse_deadzone_factor;

    bool clutch_enabled;
    bool clutch_latched;

    char profiles_dir[NAME_MAX];

    char profile_keyboard_layout[NAME_MAX];
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
void x52d_cfg_set_Mouse_Sensitivity(int param);
void x52d_cfg_set_Mouse_ReverseScroll(bool param);
void x52d_cfg_set_Mouse_IsometricMode(bool param);
void x52d_cfg_set_Mouse_CurveFactor(int param);
void x52d_cfg_set_Mouse_Deadzone(int param);
void x52d_cfg_set_Profiles_Directory(char* param);
void x52d_cfg_set_Profiles_ClutchEnabled(bool param);
void x52d_cfg_set_Profiles_ClutchLatched(bool param);
void x52d_cfg_set_Profiles_KeyboardLayout(char *param);

int x52d_config_process_kv(void *user, const char *section, const char *key, const char *value);
const char *x52d_config_get_param(struct x52d_config *cfg, const char *section, const char *key);

int x52d_config_set_defaults(struct x52d_config *cfg);

int x52d_config_load_file(struct x52d_config *cfg, const char *cfg_file);

int x52d_config_save_override(const char *override_str);

int x52d_config_apply_overrides(struct x52d_config *cfg);

void x52d_config_clear_overrides(void);

void x52d_config_load(const char *cfg_file);
void x52d_config_apply_immediate(const char *section, const char *key);
void x52d_config_apply(void);

int x52d_config_save_file(struct x52d_config *cfg, const char *cfg_file);

/** Write the full active configuration as INI to @p cfg_fp (for save or in-memory dump). */
int x52d_config_write_ini(struct x52d_config *cfg, FILE *cfg_fp, const char *path_label);

void x52d_config_save(const char *cfg_file);

int x52d_config_set(const char *section, const char *key, const char *value);
const char *x52d_config_get(const char *section, const char *key);

/**
 * Reload configuration using the canonical order: state file if present and readable,
 * else system config if present, else in-memory defaults (plus CLI overrides once).
 *
 * @return 0 on success, or a positive errno-style code on failure.
 */
int x52d_config_reload_canonical(void);

/**
 * Load defaults, then load @p path (must be readable). Applies CLI overrides.
 *
 * @return 0 on success, or a positive errno-style code on failure.
 */
int x52d_config_load_from_path(const char *path);

/**
 * Reset active configuration to defaults and re-apply CLI overrides.
 *
 * @return 0 on success, or a positive errno-style code on failure.
 */
int x52d_config_reset_to_defaults(void);

/**
 * Serialize the active configuration as INI text into a heap buffer.
 * On success, @p *out is NUL-terminated (length includes the final NUL in @p *out_len).
 *
 * @return 0 on success, or a positive errno-style code on failure (@p *out undefined).
 */
int x52d_config_dump_to_alloc(char **out, size_t *out_len);

/**
 * Atomically write the active configuration to @ref X52D_STATE_CFG_FILE
 * (temp file in the same directory + rename).
 *
 * @return 0 on success, or a positive errno-style code on failure.
 */
int x52d_config_save_state_atomic(void);

/**
 * When non-NULL, @ref x52d_config_save_session writes to this path (same file as @c x52d -c).
 * When NULL, @ref x52d_config_save_state_atomic is used.
 */
void x52d_config_set_ipc_save_path(const char *path);

/**
 * Save active configuration: session path if set, otherwise @ref x52d_config_save_state_atomic.
 *
 * @return 0 on success, or a positive errno-style code on failure.
 */
int x52d_config_save_session(void);

/**
 * Delete on-disk configuration selected by LIPC @c CONFIG_CLEAR index, then run
 * @ref x52d_config_reload_canonical. @p target is @ref X52D_CONFIG_CLEAR_TARGET_STATE or
 * @ref X52D_CONFIG_CLEAR_TARGET_SYSCONF.
 *
 * Unlink uses @c ENOENT as success (nothing to remove). If removal fails (e.g. read-only
 * sysconf), the errno is stored in @p out_unlink_errno when non-@c NULL and reload still runs.
 *
 * @return @c 0 if reload succeeded, else a positive errno-style code from reload.
 * @param out_unlink_errno optional; set to @c 0 if unlink succeeded or file was absent;
 *   otherwise set to errno from @c unlink (2); reload is still attempted.
 */
int x52d_config_clear_disk_then_reload(uint16_t target, int *out_unlink_errno);

#endif // !defined X52D_CONFIG_H
