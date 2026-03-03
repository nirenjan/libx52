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
#include <pthread.h>
#include <unistd.h>

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
#define ACTION_NAME_LEN 128
#define BUTTON_PREFIX_LEN (sizeof(BUTTON_PREFIX) - 1)
#define KEY_PREFIX      "key"
#define MACRO_PREFIX    "macro"
#define MAX_MACRO_KEYS  32
#define MAX_MACRO_STEPS 32
#define MACRO_JOB_QUEUE_SIZE 32
#define MACRO_DELAY_MS   20

typedef enum {
    ACTION_NONE,
    ACTION_KEY,
    ACTION_MACRO
} action_type_t;

typedef struct {
    action_type_t type;
    size_t key_len;         /* for ACTION_KEY: number of keys in combo */
    uint16_t *key_codes;    /* for ACTION_KEY, may be NULL */
    /* ACTION_MACRO: steps separated by |; each step is one or more keys (combo) */
    size_t macro_len;       /* total key count (flat) */
    size_t macro_step_count;
    size_t *macro_step_len; /* length of each step */
    uint16_t *macro_keys;   /* flat key codes, may be NULL */
    char action_name[ACTION_NAME_LEN];  /* optional display name (e.g. "Yaw Left") */
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
 * Parse optional " name ..." or " name \"...\"" from remainder at p into out->action_name.
 */
static void parse_optional_action_name(char *p, profile_action_t *out)
{
    char *dst;
    size_t n;

    out->action_name[0] = '\0';
    while (*p == ' ') p++;
    if (strncasecmp(p, "name", 4) != 0 || (p[4] != ' ' && p[4] != '\0')) {
        return;
    }
    p += 4;
    while (*p == ' ') p++;
    dst = out->action_name;
    n = ACTION_NAME_LEN - 1;
    if (*p == '"') {
        p++;
        while (n > 0 && *p != '\0' && *p != '"') {
            *dst++ = *p++;
            n--;
        }
    } else {
        while (n > 0 && *p != '\0' && *p != '\n' && *p != '\r') {
            *dst++ = *p++;
            n--;
        }
    }
    *dst = '\0';
    /* Trim trailing space */
    while (dst > out->action_name && (dst[-1] == ' ' || dst[-1] == '\t')) {
        *--dst = '\0';
    }
}

/**
 * Parse value into a single key code (action KEY) or macro (action MACRO).
 * Optional trailing " name Display Name" or " name \"Quoted Name\"".
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
            if (n > 0 && (size_t)(p - tok) == 4 && strncasecmp(tok, "name", 4) == 0) {
                p = tok;
                break;
            }
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
        parse_optional_action_name(p, out);
        return 0;
    }

    if (strcasecmp(tok, MACRO_PREFIX) == 0) {
        size_t step_len_buf[MAX_MACRO_STEPS];
        size_t step_count = 0;
        size_t total_keys = 0;
        size_t *step_len_alloc = NULL;
        size_t keys_in_step;
        char *segment_start;
        char *segment_end;
        char seg_buf[128];
        size_t seg_len;
        char *seg_p;
        bool has_pipe = (strchr(value, '|') != NULL);

        /* If no |, legacy format: each key is its own step (sequence of single keys) */
        if (!has_pipe) {
            n = 0;
            while (n < MAX_MACRO_KEYS && step_count < MAX_MACRO_STEPS) {
                while (*p == ' ') p++;
                if (*p == '\0') break;
                tok = (char *)p;
                while (*p != '\0' && *p != ' ') p++;
                if (n > 0 && (size_t)(p - tok) == 4 && strncasecmp(tok, "name", 4) == 0) {
                    p = tok;
                    break;
                }
                if (*p != '\0') *p++ = '\0';
                code = key_name_to_code(tok);
                if (code < 0) return -1;
                keys[n++] = (uint16_t)code;
                step_len_buf[step_count++] = 1;
                total_keys++;
            }
        } else {
            /* Steps separated by |; each step is space-separated key names (single key or combo) */
            /* First token "macro" was NUL-terminated in buf; skip past it to the rest of the value */
            p = buf;
            while (*p != '\0') p++;
            p++;
            while (*p == ' ') p++;
            while (step_count < MAX_MACRO_STEPS && total_keys < MAX_MACRO_KEYS) {
                while (*p == ' ') p++;
                if (*p == '\0') break;
                segment_start = p;
                while (*p != '\0' && *p != '|') p++;
                segment_end = p;
                if (*p == '|') p++;
                seg_len = (size_t)(segment_end - segment_start);
                while (seg_len > 0 && segment_start[seg_len - 1] == ' ') seg_len--;
                if (seg_len == 0) return -1;
                if (seg_len >= sizeof(seg_buf)) return -1;
                memcpy(seg_buf, segment_start, seg_len);
                seg_buf[seg_len] = '\0';
                seg_p = seg_buf;
                keys_in_step = 0;
                while (*seg_p != '\0') {
                    while (*seg_p == ' ') seg_p++;
                    if (*seg_p == '\0') break;
                    tok = seg_p;
                    while (*seg_p != '\0' && *seg_p != ' ') seg_p++;
                    if (*seg_p != '\0') *seg_p++ = '\0';
                    code = key_name_to_code(tok);
                    if (code < 0) return -1;
                    if (total_keys >= MAX_MACRO_KEYS) return -1;
                    keys[total_keys++] = (uint16_t)code;
                    keys_in_step++;
                }
                if (keys_in_step == 0) return -1;
                step_len_buf[step_count++] = keys_in_step;
            }
        }
        if (step_count == 0 || total_keys == 0) return -1;
        out->macro_keys = malloc(total_keys * sizeof(uint16_t));
        if (out->macro_keys == NULL) return -1;
        memcpy(out->macro_keys, keys, total_keys * sizeof(uint16_t));
        step_len_alloc = malloc(step_count * sizeof(size_t));
        if (step_len_alloc == NULL) {
            free(out->macro_keys);
            out->macro_keys = NULL;
            return -1;
        }
        memcpy(step_len_alloc, step_len_buf, step_count * sizeof(size_t));
        out->macro_len = total_keys;
        out->macro_step_len = step_len_alloc;
        out->macro_step_count = step_count;
        out->type = ACTION_MACRO;
        /* p points past last segment; parse optional name from remainder */
        parse_optional_action_name(p, out);
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
    if (a->type == ACTION_MACRO) {
        if (a->macro_keys != NULL) {
            free(a->macro_keys);
            a->macro_keys = NULL;
        }
        if (a->macro_step_len != NULL) {
            free(a->macro_step_len);
            a->macro_step_len = NULL;
        }
        a->macro_len = 0;
        a->macro_step_count = 0;
    }
    a->action_name[0] = '\0';
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

/* Macro job queue: each entry is one macro (steps; each step is single key or combo). */
struct macro_job {
    uint16_t *keys;
    size_t *step_len;
    size_t step_count;
};

static struct {
    struct macro_job ring[MACRO_JOB_QUEUE_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t drained;
    pthread_t thread;
    bool shutdown;
    bool thread_started;
} macro_queue;

static bool macro_queue_push_job(const uint16_t *keys, const size_t *step_len,
                                 size_t step_count)
{
    size_t total_keys = 0;
    size_t s;
    uint16_t *keys_copy = NULL;
    size_t *step_len_copy = NULL;
    bool ok = false;

    if (step_count == 0 || keys == NULL || step_len == NULL) {
        return false;
    }
    for (s = 0; s < step_count; s++) {
        total_keys += step_len[s];
    }
    if (total_keys == 0) {
        return false;
    }
    keys_copy = malloc(total_keys * sizeof(uint16_t));
    if (keys_copy == NULL) {
        return false;
    }
    memcpy(keys_copy, keys, total_keys * sizeof(uint16_t));
    step_len_copy = malloc(step_count * sizeof(size_t));
    if (step_len_copy == NULL) {
        free(keys_copy);
        return false;
    }
    memcpy(step_len_copy, step_len, step_count * sizeof(size_t));

    pthread_mutex_lock(&macro_queue.mutex);
    if (macro_queue.count < MACRO_JOB_QUEUE_SIZE) {
        macro_queue.ring[macro_queue.tail].keys = keys_copy;
        macro_queue.ring[macro_queue.tail].step_len = step_len_copy;
        macro_queue.ring[macro_queue.tail].step_count = step_count;
        macro_queue.tail = (macro_queue.tail + 1) % MACRO_JOB_QUEUE_SIZE;
        macro_queue.count++;
        ok = true;
        pthread_cond_signal(&macro_queue.cond);
    }
    pthread_mutex_unlock(&macro_queue.mutex);
    if (!ok) {
        free(step_len_copy);
        free(keys_copy);
    }
    return ok;
}

static void *macro_worker_thread(void *arg)
{
    (void)arg;
    for (;;) {
        struct macro_job job = { NULL, 0 };
        bool got = false;

        pthread_mutex_lock(&macro_queue.mutex);
        while (macro_queue.count == 0 && !macro_queue.shutdown) {
            pthread_cond_wait(&macro_queue.cond, &macro_queue.mutex);
        }
        if (macro_queue.shutdown && macro_queue.count == 0) {
            pthread_cond_broadcast(&macro_queue.drained);
            pthread_mutex_unlock(&macro_queue.mutex);
            break;
        }
        if (macro_queue.count > 0) {
            job = macro_queue.ring[macro_queue.head];
            macro_queue.ring[macro_queue.head].keys = NULL;
            macro_queue.ring[macro_queue.head].step_len = NULL;
            macro_queue.ring[macro_queue.head].step_count = 0;
            macro_queue.head = (macro_queue.head + 1) % MACRO_JOB_QUEUE_SIZE;
            macro_queue.count--;
            got = true;
        }
        pthread_mutex_unlock(&macro_queue.mutex);

        if (got && job.keys != NULL && job.step_len != NULL) {
            size_t offset = 0;
            size_t s;
            for (s = 0; s < job.step_count; s++) {
                size_t len = job.step_len[s];
                size_t i;
                /* Combo: all keys down in order, delay, all keys up in reverse */
                for (i = 0; i < len; i++) {
                    x52d_keyboard_evdev_key(job.keys[offset + i], 1);
                }
                usleep((useconds_t)MACRO_DELAY_MS * 1000);
                for (i = len; i > 0; i--) {
                    x52d_keyboard_evdev_key(job.keys[offset + i - 1], 0);
                }
                offset += len;
                usleep((useconds_t)MACRO_DELAY_MS * 1000);
            }
            free(job.step_len);
            free(job.keys);
            /* Signal drained only after all keys emitted, so wait_drained is accurate */
            pthread_mutex_lock(&macro_queue.mutex);
            pthread_cond_broadcast(&macro_queue.drained);
            pthread_mutex_unlock(&macro_queue.mutex);
        }
    }
    pthread_mutex_lock(&macro_queue.mutex);
    pthread_cond_broadcast(&macro_queue.drained);
    pthread_mutex_unlock(&macro_queue.mutex);
    return NULL;
}

void x52d_profile_init(void)
{
    int rc;

    macro_queue.head = 0;
    macro_queue.tail = 0;
    macro_queue.count = 0;
    macro_queue.shutdown = false;
    rc = pthread_mutex_init(&macro_queue.mutex, NULL);
    if (rc != 0) {
        PINELOG_ERROR(_("Failed to create macro queue mutex: %s"), strerror(rc));
    } else {
        rc = pthread_cond_init(&macro_queue.cond, NULL);
        if (rc != 0) {
            pthread_mutex_destroy(&macro_queue.mutex);
            PINELOG_ERROR(_("Failed to create macro queue cond: %s"), strerror(rc));
        } else if (pthread_cond_init(&macro_queue.drained, NULL) != 0) {
            pthread_cond_destroy(&macro_queue.cond);
            pthread_mutex_destroy(&macro_queue.mutex);
            PINELOG_ERROR(_("Failed to create macro queue drained cond"));
        } else {
            rc = pthread_create(&macro_queue.thread, NULL, macro_worker_thread, NULL);
            if (rc != 0) {
                pthread_cond_destroy(&macro_queue.drained);
                pthread_cond_destroy(&macro_queue.cond);
                pthread_mutex_destroy(&macro_queue.mutex);
                PINELOG_ERROR(_("Failed to start macro worker thread: %s"), strerror(rc));
            } else {
                macro_queue.thread_started = true;
            }
        }
    }
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
    if (macro_queue.thread_started) {
        pthread_mutex_lock(&macro_queue.mutex);
        macro_queue.shutdown = true;
        pthread_cond_broadcast(&macro_queue.cond);
        pthread_mutex_unlock(&macro_queue.mutex);
        pthread_join(macro_queue.thread, NULL);
        pthread_cond_destroy(&macro_queue.drained);
        pthread_cond_destroy(&macro_queue.cond);
        pthread_mutex_destroy(&macro_queue.mutex);
        macro_queue.thread_started = false;
    }
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

void x52d_profile_macro_wait_drained(void)
{
    if (!macro_queue.thread_started) {
        return;
    }
    pthread_mutex_lock(&macro_queue.mutex);
    while (macro_queue.count > 0) {
        pthread_cond_wait(&macro_queue.drained, &macro_queue.mutex);
    }
    pthread_mutex_unlock(&macro_queue.mutex);
}

const char *x52d_profile_get_action_name(const libx52io_report *report,
                                        libx52io_button btn)
{
    unsigned int chain_index;
    int layer;
    const profile_action_t *a;
    int i;

    if (!profile_loaded || report == NULL || btn >= LIBX52IO_BUTTON_MAX) {
        return NULL;
    }
    chain_index = get_layer_index(report);
    for (i = 0; i < MAX_FALLBACK; i++) {
        layer = fallback_chain[chain_index][i];
        if (layer < 0) {
            break;
        }
        a = &layers[layer][btn];
        if (a->type != ACTION_NONE) {
            return (a->action_name[0] != '\0') ? a->action_name : NULL;
        }
    }
    return NULL;
}

static void emit_macro(const profile_action_t *a)
{
    if (a->type != ACTION_MACRO || a->macro_keys == NULL || a->macro_step_len == NULL ||
        !x52d_keyboard_evdev_available()) {
        return;
    }
    if (!macro_queue_push_job(a->macro_keys, a->macro_step_len, a->macro_step_count)) {
        PINELOG_WARN(_("Macro queue full, dropping macro"));
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
