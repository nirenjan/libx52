/*
 * Saitek X52 Pro MFD & LED driver - Profile (button-to-keyboard mapping)
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#include "libevdev/libevdev.h"
#include "libx52io.h"
#include "ini.h"

#define PINELOG_MODULE X52D_MOD_PROFILE
#include "pinelog.h"

#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_keyboard.h"
#include "x52d_profile.h"

#define NUM_LAYERS      6   /* 3 modes × 2 (no shift / shift) */
#define MAX_FALLBACK    4   /* max layers in a fallback chain */
#define BUTTON_PREFIX   "Button."
#define PROFILE_SECTION "Profile"
#define PROFILE_NAME_KEY "Name"
#define SHIFT_BUTTON_KEY "ShiftButton"
#define DEFAULT_SHIFT_BUTTON "BTN_PINKY"
#define PROFILE_NAME_LEN 128
#define SHIFT_BUTTON_STR_LEN 64
#define BUTTON_PREFIX_LEN (sizeof(BUTTON_PREFIX) - 1)
#define KEY_PREFIX      "key"
#define MACRO_PREFIX    "macro"
#define MAX_MACRO_KEYS  32

typedef enum {
    ACTION_NONE,
    ACTION_KEY,
    ACTION_MACRO
} action_type_t;

typedef struct {
    action_type_t type;
    size_t key_len;         /* for ACTION_KEY: number of keys in combo */
    uint16_t *key_codes;    /* for ACTION_KEY, may be NULL */
    size_t macro_len;
    uint16_t *macro_keys;   /* for ACTION_MACRO, may be NULL */
} profile_action_t;

static profile_action_t layers[NUM_LAYERS][LIBX52IO_BUTTON_MAX];
static int shift_button_index = -1;  /* -1 = no shift button resolved */
static bool profile_loaded = false;
/* [Profile] section options; set during parse, resolved after. */
static char profile_name_str[PROFILE_NAME_LEN];
static char profile_shift_button_str[SHIFT_BUTTON_STR_LEN];

static int button_name_to_index(const char *name)
{
    libx52io_button b;
    const char *str;

    if (name == NULL || *name == '\0') {
        return -1;
    }
    for (b = LIBX52IO_BTN_TRIGGER; b < LIBX52IO_BUTTON_MAX; b++) {
        str = libx52io_button_to_str(b);
        if (str != NULL && strcasecmp(str, name) == 0) {
            return (int)b;
        }
    }
    return -1;
}

static int key_name_to_code(const char *name)
{
    int code;

    if (name == NULL || *name == '\0') {
        return -1;
    }
    code = libevdev_event_code_from_name(EV_KEY, name);
    return (code >= 0) ? code : -1;
}

/**
 * Parse value into a single key code (action KEY) or macro (action MACRO).
 * Returns 0 on success, -1 on parse error.
 */
static int parse_action_value(const char *value, profile_action_t *out)
{
    char buf[256];
    char *tok;
    char *p;
    uint16_t keys[MAX_MACRO_KEYS];
    size_t n = 0;
    int code;

    memset(out, 0, sizeof(*out));
    out->type = ACTION_NONE;

    if (value == NULL) {
        return -1;
    }
    strncpy(buf, value, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    p = buf;
    while (*p == ' ') p++;
    if (*p == '\0') {
        return -1;
    }

    tok = (char *)p;
    while (*p != '\0' && *p != ' ') p++;
    if (*p != '\0') {
        *p = '\0';
        p++;
    }

    if (strcasecmp(tok, KEY_PREFIX) == 0) {
        /* Parse key combo: key KEY_MOD KEY_KEY [KEY_KEY ...] e.g. key KEY_LEFTCTRL KEY_X */
        n = 0;
        while (n < MAX_MACRO_KEYS) {
            while (*p == ' ') p++;
            if (*p == '\0') break;
            tok = (char *)p;
            while (*p != '\0' && *p != ' ') p++;
            if (*p != '\0') { *p = '\0'; p++; }
            code = key_name_to_code(tok);
            if (code < 0) return -1;
            keys[n++] = (uint16_t)code;
        }
        if (n == 0) return -1;
        out->key_codes = malloc(n * sizeof(uint16_t));
        if (out->key_codes == NULL) return -1;
        memcpy(out->key_codes, keys, n * sizeof(uint16_t));
        out->key_len = n;
        out->type = ACTION_KEY;
        return 0;
    }

    if (strcasecmp(tok, MACRO_PREFIX) == 0) {
        while (n < MAX_MACRO_KEYS) {
            while (*p == ' ') p++;
            if (*p == '\0') {
                break;
            }
            tok = (char *)p;
            while (*p != '\0' && *p != ' ') p++;
            if (*p != '\0') {
                *p = '\0';
                p++;
            }
            code = key_name_to_code(tok);
            if (code < 0) {
                return -1;
            }
            keys[n++] = (uint16_t)code;
        }
        if (n == 0) {
            return -1;
        }
        out->macro_keys = malloc(n * sizeof(uint16_t));
        if (out->macro_keys == NULL) {
            return -1;
        }
        memcpy(out->macro_keys, keys, n * sizeof(uint16_t));
        out->macro_len = n;
        out->type = ACTION_MACRO;
        return 0;
    }

    return -1;
}

static int section_to_layer(const char *section)
{
    if (section == NULL) {
        return -1;
    }
    if (strcasecmp(section, "Mode1") == 0) return 0;
    if (strcasecmp(section, "Mode2") == 0) return 2;
    if (strcasecmp(section, "Mode3") == 0) return 4;
    if (strcasecmp(section, "Mode1.Shift") == 0) return 1;
    if (strcasecmp(section, "Mode2.Shift") == 0) return 3;
    if (strcasecmp(section, "Mode3.Shift") == 0) return 5;
    return -1;
}

static void free_action(profile_action_t *a)
{
    if (a->type == ACTION_KEY && a->key_codes != NULL) {
        free(a->key_codes);
        a->key_codes = NULL;
        a->key_len = 0;
    }
    if (a->type == ACTION_MACRO && a->macro_keys != NULL) {
        free(a->macro_keys);
        a->macro_keys = NULL;
        a->macro_len = 0;
    }
    a->type = ACTION_NONE;
}

static int profile_ini_handler(void *user, const char *section, const char *key,
                              const char *value)
{
    int layer;
    int btn;
    profile_action_t action;
    profile_action_t (*arr)[NUM_LAYERS][LIBX52IO_BUTTON_MAX] = user;

    /* [Profile] section: profile-wide options (Name, ShiftButton) */
    if (section != NULL && strcasecmp(section, PROFILE_SECTION) == 0) {
        if (key != NULL && value != NULL) {
            if (strcasecmp(key, PROFILE_NAME_KEY) == 0) {
                strncpy(profile_name_str, value, PROFILE_NAME_LEN - 1);
                profile_name_str[PROFILE_NAME_LEN - 1] = '\0';
            } else if (strcasecmp(key, SHIFT_BUTTON_KEY) == 0) {
                strncpy(profile_shift_button_str, value, SHIFT_BUTTON_STR_LEN - 1);
                profile_shift_button_str[SHIFT_BUTTON_STR_LEN - 1] = '\0';
            }
        }
        return 1;
    }

    layer = section_to_layer(section);
    if (layer < 0) {
        return 1;   /* unknown section, ignore */
    }

    if (key == NULL || value == NULL) {
        return 1;
    }
    if (strncasecmp(key, BUTTON_PREFIX, BUTTON_PREFIX_LEN) != 0) {
        return 1;
    }
    key += BUTTON_PREFIX_LEN;
    btn = button_name_to_index(key);
    if (btn < 0) {
        PINELOG_INFO(_("Unknown button name in profile: %s"), key);
        return 1;
    }

    if (parse_action_value(value, &action) != 0) {
        PINELOG_INFO(_("Invalid action value for %s.%s: %s"), section, key, value);
        return 1;
    }

    free_action(&(*arr)[layer][btn]);
    (*arr)[layer][btn] = action;
    return 1;
}

static void clear_all_layers(void)
{
    int layer;
    libx52io_button btn;

    for (layer = 0; layer < NUM_LAYERS; layer++) {
        for (btn = LIBX52IO_BTN_TRIGGER; btn < LIBX52IO_BUTTON_MAX; btn++) {
            free_action(&layers[layer][btn]);
        }
    }
}

static void load_profile(void)
{
    const char *dir;
    const char *name;
    char path[PATH_MAX];
    int rc;

    clear_all_layers();
    profile_loaded = false;
    shift_button_index = -1;
    profile_name_str[0] = '\0';
    profile_shift_button_str[0] = '\0';

    dir = x52d_config_get("Profiles", "Directory");
    name = x52d_config_get("Profiles", "Profile");
    if (dir == NULL || *dir == '\0' || name == NULL || *name == '\0') {
        PINELOG_INFO(_("Profile directory or name not set, using empty profile"));
        return;
    }

    rc = snprintf(path, sizeof(path), "%s/%s.conf", dir, name);
    if (rc < 0 || (size_t)rc >= sizeof(path)) {
        PINELOG_ERROR(_("Profile path too long"));
        return;
    }

    rc = ini_parse(path, profile_ini_handler, &layers);
    if (rc < 0) {
        PINELOG_ERROR(_("Failed to load profile %s: %s"), path, strerror(errno));
        return;
    }
    if (rc > 0) {
        PINELOG_WARN(_("Profile %s had %d parse errors"), path, rc);
    }

    profile_loaded = true;

    if (profile_name_str[0] != '\0') {
        PINELOG_INFO(_("Loaded profile: %s"), profile_name_str);
    }

    /* Resolve shift button from [Profile] ShiftButton; default BTN_PINKY if not set */
    name = (profile_shift_button_str[0] != '\0')
           ? profile_shift_button_str
           : DEFAULT_SHIFT_BUTTON;
    shift_button_index = button_name_to_index(name);
    if (shift_button_index < 0) {
        PINELOG_WARN(_("Unknown ShiftButton '%s' in profile, shift disabled"), name);
    }
}

void x52d_profile_init(void)
{
    load_profile();
    PINELOG_INFO(_("Profile module initialized"));
}

const char *x52d_profile_get_name(void)
{
    if (!profile_loaded || profile_name_str[0] == '\0') {
        return NULL;
    }
    return profile_name_str;
}

void x52d_profile_exit(void)
{
    clear_all_layers();
    shift_button_index = -1;
    profile_name_str[0] = '\0';
    profile_shift_button_str[0] = '\0';
    profile_loaded = false;
    PINELOG_INFO(_("Profile module shut down"));
}

/*
 * Fallback chain per (mode, shift): try these layer indices in order;
 * use the first that has a mapping for the button. -1 terminates.
 * Layer order: 0=Mode1, 1=Mode1.Shift, 2=Mode2, 3=Mode2.Shift, 4=Mode3, 5=Mode3.Shift.
 * - Shift: ModeN.Shift falls back to ModeN.
 * - Mode: Mode2 falls back to Mode1; Mode3 falls back to Mode2 then Mode1.
 */
static const int fallback_chain[NUM_LAYERS][MAX_FALLBACK] = {
    { 0, -1, -1, -1 },   /* mode 1, no shift: Mode1 only */
    { 1, 0, -1, -1 },    /* mode 1, shift: Mode1.Shift then Mode1 */
    { 2, 0, -1, -1 },    /* mode 2, no shift: Mode2 then Mode1 */
    { 3, 2, 0, -1 },     /* mode 2, shift: Mode2.Shift then Mode2 then Mode1 */
    { 4, 2, 0, -1 },     /* mode 3, no shift: Mode3 then Mode2 then Mode1 */
    { 5, 4, 2, 0 },      /* mode 3, shift: Mode3.Shift then Mode3 then Mode2 then Mode1 */
};

static unsigned int get_layer_index(const libx52io_report *report)
{
    unsigned int mode;
    bool shift;

    mode = report->mode;
    if (mode < 1) {
        mode = 1;
    }
    if (mode > 3) {
        mode = 3;
    }
    shift = (shift_button_index >= 0 &&
             shift_button_index < LIBX52IO_BUTTON_MAX &&
             report->button[shift_button_index]);
    return (mode - 1u) * 2u + (shift ? 1u : 0u);
}

/* Return the first non-NONE action for btn in the fallback chain for this report. */
static const profile_action_t *get_action_for_button(const libx52io_report *report,
                                                     libx52io_button btn)
{
    unsigned int chain_index;
    int layer;
    const profile_action_t *a;
    int i;

    chain_index = get_layer_index(report);
    for (i = 0; i < MAX_FALLBACK; i++) {
        layer = fallback_chain[chain_index][i];
        if (layer < 0) {
            break;
        }
        a = &layers[layer][btn];
        if (a->type != ACTION_NONE) {
            return a;
        }
    }
    return NULL;
}

static void emit_macro(const profile_action_t *a)
{
    size_t i;

    if (a->type != ACTION_MACRO || a->macro_keys == NULL || !x52d_keyboard_evdev_available()) {
        return;
    }
    for (i = 0; i < a->macro_len; i++) {
        x52d_keyboard_evdev_key(a->macro_keys[i], 1);
        x52d_keyboard_evdev_key(a->macro_keys[i], 0);
    }
}

void x52d_profile_apply(const libx52io_report *report,
                        const libx52io_report *prev)
{
    libx52io_button btn;
    const profile_action_t *a;
    bool pressed;
    bool was_pressed;

    if (!profile_loaded || !x52d_keyboard_evdev_available()) {
        return;
    }

    for (btn = LIBX52IO_BTN_TRIGGER; btn < LIBX52IO_BUTTON_MAX; btn++) {
        was_pressed = prev->button[btn];
        pressed = report->button[btn];
        if (pressed == was_pressed) {
            continue;
        }

        a = get_action_for_button(report, btn);
        if (a == NULL) {
            continue;
        }

        if (pressed) {
            if (a->type == ACTION_KEY && a->key_codes != NULL) {
                size_t i;
                for (i = 0; i < a->key_len; i++) {
                    x52d_keyboard_evdev_key(a->key_codes[i], 1);
                }
            } else if (a->type == ACTION_MACRO) {
                emit_macro(a);
            }
        } else {
            if (a->type == ACTION_KEY && a->key_codes != NULL) {
                size_t i;
                for (i = a->key_len; i > 0; i--) {
                    x52d_keyboard_evdev_key(a->key_codes[i - 1], 0);
                }
            }
            /* macro: nothing on release */
        }
    }
}
