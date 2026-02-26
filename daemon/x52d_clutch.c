/*
 * Saitek X52 Pro MFD & LED driver - Clutch (profile selection UI)
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "libx52.h"
#include "libx52io.h"

#define PINELOG_MODULE X52D_MOD_CLUTCH
#include "pinelog.h"

#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_device.h"
#include "x52d_clutch.h"
#include "x52d_profile.h"

#define MFD_LINE_SIZE      16
#define MFD_LINES          3
#define MAX_PROFILES       32
#define MAX_PROFILE_NAME   16  /* truncate for MFD display */

static bool clutch_engaged;
static char *profile_list[MAX_PROFILES];
static int profile_count;
static int highlight_index;
static bool clutch_ui_active;     /* true after we've shown the list at least once */

static bool config_clutch_enabled(void)
{
    const char *v = x52d_config_get("Profiles", "ClutchEnabled");
    if (v == NULL) return false;
    return (strcasecmp(v, "yes") == 0 || strcasecmp(v, "true") == 0);
}

static bool config_clutch_latched(void)
{
    const char *v = x52d_config_get("Profiles", "ClutchLatched");
    if (v == NULL) return false;
    return (strcasecmp(v, "yes") == 0 || strcasecmp(v, "true") == 0);
}

static void clutch_update_led(bool enabled, bool engaged)
{
    if (!enabled) {
        x52d_dev_set_blink(0);
        x52d_dev_set_led_state(LIBX52_LED_CLUTCH, LIBX52_LED_STATE_RED);
        return;
    }
    if (!engaged) {
        x52d_dev_set_blink(0);
        x52d_dev_set_led_state(LIBX52_LED_CLUTCH, LIBX52_LED_STATE_GREEN);
        return;
    }
    /* Engaged: amber with hardware blink */
    x52d_dev_set_led_state(LIBX52_LED_CLUTCH, LIBX52_LED_STATE_AMBER);
    x52d_dev_set_blink(1);
}

static void free_profile_list(void)
{
    int i;
    for (i = 0; i < profile_count; i++) {
        free(profile_list[i]);
        profile_list[i] = NULL;
    }
    profile_count = 0;
}

static int scan_profiles(void)
{
    const char *dir_path;
    DIR *dir;
    struct dirent *ent;
    size_t len;
    char *stem;

    free_profile_list();
    dir_path = x52d_config_get("Profiles", "Directory");
    if (dir_path == NULL || dir_path[0] == '\0') {
        return 0;
    }
    dir = opendir(dir_path);
    if (dir == NULL) {
        PINELOG_WARN(_("Cannot open profiles directory %s: %s"), dir_path, strerror(errno));
        return 0;
    }
    while (profile_count < MAX_PROFILES && (ent = readdir(dir)) != NULL) {
        len = strlen(ent->d_name);
        if (len <= 4) continue;
        if (strcasecmp(ent->d_name + len - 4, ".conf") != 0) continue;
        stem = malloc(len - 4);  /* strip .conf, need len-5+1 for NUL */
        if (stem == NULL) continue;
        memcpy(stem, ent->d_name, len - 5);
        stem[len - 5] = '\0';
        if (stem[0] == '\0') {
            free(stem);
            continue;
        }
        profile_list[profile_count++] = stem;
    }
    closedir(dir);
    /* Sort by name (simple bubble sort) */
    {
        int i, j;
        for (i = 0; i < profile_count - 1; i++) {
            for (j = i + 1; j < profile_count; j++) {
                if (strcasecmp(profile_list[i], profile_list[j]) > 0) {
                    char *t = profile_list[i];
                    profile_list[i] = profile_list[j];
                    profile_list[j] = t;
                }
            }
        }
    }
    return profile_count;
}

static void clutch_clear_mfd(void)
{
    static const char blank[MFD_LINE_SIZE] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                                               ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    x52d_dev_set_text(0, blank, MFD_LINE_SIZE);
    x52d_dev_set_text(1, blank, MFD_LINE_SIZE);
    x52d_dev_set_text(2, blank, MFD_LINE_SIZE);
}

static void clutch_refresh_mfd(void)
{
    char line_buf[MFD_LINE_SIZE];
    int i;
    size_t len;

    if (profile_count == 0) {
        clutch_clear_mfd();
        return;
    }
    if (highlight_index >= profile_count) highlight_index = profile_count - 1;
    if (highlight_index < 0) highlight_index = 0;

    for (i = 0; i < MFD_LINES; i++) {
        int idx = highlight_index - 1 + i;  /* line 0 = above, 1 = current, 2 = below */
        if (idx < 0 || idx >= profile_count) {
            memset(line_buf, ' ', MFD_LINE_SIZE);
            x52d_dev_set_text(i, line_buf, MFD_LINE_SIZE);
            continue;
        }
        len = strlen(profile_list[idx]);
        if (len > MAX_PROFILE_NAME) len = MAX_PROFILE_NAME;
        memset(line_buf, ' ', MFD_LINE_SIZE);
        if (idx == highlight_index) {
            line_buf[0] = '>';
            if (len + 1 < MFD_LINE_SIZE) {
                memcpy(line_buf + 1, profile_list[idx], len);
                len++;
            } else {
                memcpy(line_buf + 1, profile_list[idx], MFD_LINE_SIZE - 1);
                len = MFD_LINE_SIZE;
            }
        } else {
            memcpy(line_buf, profile_list[idx], len);
        }
        x52d_dev_set_text(i, line_buf, MFD_LINE_SIZE);
    }
}

static void clutch_enter_ui(void)
{
    scan_profiles();
    highlight_index = 0;
    clutch_ui_active = true;
    clutch_refresh_mfd();
}

static void clutch_exit_ui(bool restore_mfd)
{
    clutch_ui_active = false;
    if (restore_mfd) {
        clutch_clear_mfd();
    }
    free_profile_list();
}

static void clutch_do_clear(void)
{
    clutch_exit_ui(true);
    clutch_engaged = false;
}

static void clutch_do_select(void)
{
    if (profile_count > 0 && highlight_index >= 0 && highlight_index < profile_count) {
        const char *name = profile_list[highlight_index];
        if (x52d_config_set("Profiles", "Profile", name) == 0) {
            x52d_profile_init();
            PINELOG_INFO(_("Profile switched to: %s"), name);
        }
    }
    clutch_exit_ui(true);
    clutch_engaged = false;
}

bool x52d_clutch_process(const libx52io_report *report,
                         const libx52io_report *prev)
{
    bool enabled = config_clutch_enabled();
    bool latched = config_clutch_latched();
    bool clutch_pressed = report->button[LIBX52IO_BTN_CLUTCH];
    bool prev_clutch = prev->button[LIBX52IO_BTN_CLUTCH];

    /* Update clutch_engaged */
    if (latched) {
        if (prev_clutch && !clutch_pressed) {
            clutch_engaged = !clutch_engaged;
        }
    } else {
        clutch_engaged = clutch_pressed;
    }

    /* Clutch LED: disabled=red, enabled+not engaged=green, engaged=blink amber/off 250ms */
    clutch_update_led(enabled, clutch_engaged);

    if (!enabled) {
        return false;
    }

    if (!clutch_engaged) {
        if (clutch_ui_active) {
            clutch_exit_ui(true);
        }
        return false;
    }

    /* Clutch engaged: show UI and handle hat/swipe */
    if (!clutch_ui_active) {
        clutch_enter_ui();
    }

    /*
     * Safety: if both HATX and HATY are non-zero (diagonal), use HATY alone
     * and treat HATX as 0 to avoid accidental clear/select on diagonals.
     */
    {
        int32_t prev_x = prev->axis[LIBX52IO_AXIS_HATX];
        int32_t prev_y = prev->axis[LIBX52IO_AXIS_HATY];
        int32_t cur_x = report->axis[LIBX52IO_AXIS_HATX];
        int32_t cur_y = report->axis[LIBX52IO_AXIS_HATY];
        if (prev_x != 0 && prev_y != 0) prev_x = 0;
        if (cur_x != 0 && cur_y != 0) cur_x = 0;
        /* Edge detection: HATY -1=up, HATY 1=down, HATX -1=left, HATX 1=right */
        if (prev_x != -1 && cur_x == -1) {
            clutch_do_clear();
            return true;
        }
        if (profile_count > 0) {
            if (prev_y != -1 && cur_y == -1) {
                highlight_index--;
                if (highlight_index < 0) highlight_index = profile_count - 1;
                clutch_refresh_mfd();
            }
            if (prev_y != 1 && cur_y == 1) {
                highlight_index++;
                if (highlight_index >= profile_count) highlight_index = 0;
                clutch_refresh_mfd();
            }
            if (prev_x != 1 && cur_x == 1) {
                clutch_do_select();
                return true;
            }
        }
    }

    return true;  /* consumed: in clutch UI, skip profile/mouse */
}
