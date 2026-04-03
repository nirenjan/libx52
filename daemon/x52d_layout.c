/*
 * Saitek X52 Pro MFD & LED driver — keyboard layout (.layout) loader
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/*
 * INI grammar (UTF-8 text):
 *
 *   [Layout]
 *   Name=<short id>          # e.g. us
 *   Description=<optional>
 *
 *   [<one UTF-8 code point>] | [U+XXXX] | [u+XXXX]
 *   Key=<vkm_key name>|0xNN  # HID usage (page 0x07), hex or symbolic (e.g. A, Enter)
 *   Mods=<modifier list>    # optional; empty = none. Tokens: Shift LeftShift RightShift
 *                           # Ctrl LeftCtrl RightCtrl Alt LeftAlt RightAlt
 *                           # Gui LeftGui RightGui Super Meta
 *                           # Separate tokens with ',' or '+' (whitespace trimmed).
 *
 * Section names (except Layout) must encode exactly one Unicode scalar value.
 * Later entries replace earlier ones for the same code point.
 */

#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define PINELOG_MODULE X52D_MOD_LAYOUT
#include "ini.h"
#include "pinelog.h"
#include "x52d_const.h"
#include "x52d_layout.h"

struct x52d_layout_entry {
    uint32_t cp;
    vkm_key key;
    vkm_key_modifiers mods;
};

struct x52d_layout {
    char *name;
    char *description;
    struct x52d_layout_entry *entries;
    size_t n_entries;
};

void x52d_layout_free(struct x52d_layout *layout)
{
    if (layout == NULL) {
        return;
    }
    free(layout->name);
    free(layout->description);
    free(layout->entries);
    free(layout);
}

const char *x52d_layout_get_name(const struct x52d_layout *layout)
{
    if (layout == NULL || layout->name == NULL) {
        return "";
    }
    return layout->name;
}

static int cmp_entry_cp(const void *a, const void *b)
{
    const struct x52d_layout_entry *ea = a;
    const struct x52d_layout_entry *eb = b;
    if (ea->cp < eb->cp) {
        return -1;
    }
    if (ea->cp > eb->cp) {
        return 1;
    }
    return 0;
}

bool x52d_layout_lookup(const struct x52d_layout *layout, uint32_t cp,
                        struct x52d_layout_recipe *out_recipe)
{
    struct x52d_layout_entry key = {.cp = cp};
    struct x52d_layout_entry *found;

    if (layout == NULL || out_recipe == NULL || layout->entries == NULL ||
        layout->n_entries == 0U) {
        return false;
    }

    found = bsearch(&key, layout->entries, layout->n_entries, sizeof(key), cmp_entry_cp);
    if (found == NULL) {
        return false;
    }
    out_recipe->key = found->key;
    out_recipe->mods = found->mods;
    return true;
}

static int utf8_decode_one(const char *s, uint32_t *out_cp, size_t *out_len)
{
    const unsigned char *u = (const unsigned char *)s;
    uint32_t cp;
    size_t n;

    if (s == NULL || out_cp == NULL || out_len == NULL) {
        return EINVAL;
    }
    if (u[0] < 0x80U) {
        if (u[0] == 0U) {
            return EINVAL;
        }
        *out_cp = u[0];
        *out_len = 1U;
        return 0;
    }
    if ((u[0] & 0xE0U) == 0xC0U) {
        if ((u[0] & 0x1EU) == 0U) {
            return EINVAL; /* overlong */
        }
        if (u[1] == 0U || (u[1] &0xC0U) != 0x80U) {
            return EINVAL;
        }
        cp = (uint32_t)(u[0] & 0x1FU) << 6 | (uint32_t)(u[1] & 0x3FU);
        n = 2U;
    } else if ((u[0] & 0xF0U) == 0xE0U) {
        if (u[1] == 0U || u[2] == 0U) {
            return EINVAL;
        }
        if ((u[1] & 0xC0U) != 0x80U || (u[2] & 0xC0U) != 0x80U) {
            return EINVAL;
        }
        cp = (uint32_t)(u[0] & 0x0FU) << 12 | (uint32_t)(u[1] & 0x3FU) << 6 |
             (uint32_t)(u[2] & 0x3FU);
        n = 3U;
        if (cp < 0x800U) {
            return EINVAL;
        }
    } else if ((u[0] & 0xF8U) == 0xF0U) {
        if (u[1] == 0U || u[2] == 0U || u[3] == 0U) {
            return EINVAL;
        }
        if ((u[1] & 0xC0U) != 0x80U || (u[2] & 0xC0U) != 0x80U ||
            (u[3] & 0xC0U) != 0x80U) {
            return EINVAL;
        }
        cp = (uint32_t)(u[0] & 0x07U) << 18 | (uint32_t)(u[1] & 0x3FU) << 12 |
             (uint32_t)(u[2] & 0x3FU) << 6 | (uint32_t)(u[3] & 0x3FU);
        n = 4U;
        if (cp < 0x10000U || cp > 0x10FFFFU) {
            return EINVAL;
        }
    } else {
        return EINVAL;
    }

    *out_cp = cp;
    *out_len = n;
    return 0;
}

static void trim_inplace(char *s)
{
    char *end;
    size_t i;

    if (s == NULL) {
        return;
    }
    i = 0;
    while (s[i] != '\0' && isspace((unsigned char)s[i])) {
        i++;
    }
    if (i > 0U) {
        memmove(s, s + i, strlen(s + i) + 1U);
    }
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) {
        end--;
        *end = '\0';
    }
}

static const struct vkm_key_name {
    const char *name;
    vkm_key key;
} vkm_key_names[] = {
    {"0", VKM_KEY_0},           {"1", VKM_KEY_1},
    {"2", VKM_KEY_2},           {"3", VKM_KEY_3},
    {"4", VKM_KEY_4},           {"5", VKM_KEY_5},
    {"6", VKM_KEY_6},           {"7", VKM_KEY_7},
    {"8", VKM_KEY_8},           {"9", VKM_KEY_9},
    {"A", VKM_KEY_A},           {"Application", VKM_KEY_APPLICATION},
    {"Apostrophe", VKM_KEY_APOSTROPHE},
    {"B", VKM_KEY_B},           {"Backslash", VKM_KEY_BACKSLASH},
    {"Backspace", VKM_KEY_BACKSPACE},
    {"C", VKM_KEY_C},           {"CapsLock", VKM_KEY_CAPS_LOCK},
    {"Comma", VKM_KEY_COMMA},   {"D", VKM_KEY_D},
    {"Delete", VKM_KEY_DELETE_FORWARD},
    {"DownArrow", VKM_KEY_DOWN_ARROW},
    {"E", VKM_KEY_E},           {"End", VKM_KEY_END},
    {"Enter", VKM_KEY_ENTER},   {"Equal", VKM_KEY_EQUAL},
    {"Escape", VKM_KEY_ESCAPE}, {"F", VKM_KEY_F},
    {"F1", VKM_KEY_F1},         {"F10", VKM_KEY_F10},
    {"F11", VKM_KEY_F11},       {"F12", VKM_KEY_F12},
    {"F2", VKM_KEY_F2},         {"F3", VKM_KEY_F3},
    {"F4", VKM_KEY_F4},         {"F5", VKM_KEY_F5},
    {"F6", VKM_KEY_F6},         {"F7", VKM_KEY_F7},
    {"F8", VKM_KEY_F8},         {"F9", VKM_KEY_F9},
    {"G", VKM_KEY_G},           {"GraveAccent", VKM_KEY_GRAVE_ACCENT},
    {"H", VKM_KEY_H},           {"Home", VKM_KEY_HOME},
    {"I", VKM_KEY_I},           {"Insert", VKM_KEY_INSERT},
    {"IntlBackslash", VKM_KEY_INTL_BACKSLASH},
    {"J", VKM_KEY_J},           {"K", VKM_KEY_K},
    {"L", VKM_KEY_L},           {"LeftAlt", VKM_KEY_LEFT_ALT},
    {"LeftArrow", VKM_KEY_LEFT_ARROW},
    {"LeftBracket", VKM_KEY_LEFT_BRACKET},
    {"LeftCtrl", VKM_KEY_LEFT_CTRL},
    {"LeftGui", VKM_KEY_LEFT_GUI},
    {"LeftShift", VKM_KEY_LEFT_SHIFT},
    {"M", VKM_KEY_M},           {"Minus", VKM_KEY_MINUS},
    {"N", VKM_KEY_N},           {"NonUsHash", VKM_KEY_NONUS_HASH},
    {"O", VKM_KEY_O},           {"P", VKM_KEY_P},
    {"PageDown", VKM_KEY_PAGE_DOWN},
    {"PageUp", VKM_KEY_PAGE_UP}, {"Pause", VKM_KEY_PAUSE},
    {"Period", VKM_KEY_PERIOD}, {"PrintScreen", VKM_KEY_PRINT_SCREEN},
    {"Q", VKM_KEY_Q},           {"R", VKM_KEY_R},
    {"Return", VKM_KEY_ENTER},  {"RightAlt", VKM_KEY_RIGHT_ALT},
    {"RightArrow", VKM_KEY_RIGHT_ARROW},
    {"RightBracket", VKM_KEY_RIGHT_BRACKET},
    {"RightCtrl", VKM_KEY_RIGHT_CTRL},
    {"RightGui", VKM_KEY_RIGHT_GUI},
    {"RightShift", VKM_KEY_RIGHT_SHIFT},
    {"S", VKM_KEY_S},           {"ScrollLock", VKM_KEY_SCROLL_LOCK},
    {"Semicolon", VKM_KEY_SEMICOLON},
    {"Slash", VKM_KEY_SLASH},   {"Space", VKM_KEY_SPACE},
    {"T", VKM_KEY_T},           {"Tab", VKM_KEY_TAB},
    {"U", VKM_KEY_U},           {"UpArrow", VKM_KEY_UP_ARROW},
    {"V", VKM_KEY_V},           {"W", VKM_KEY_W},
    {"X", VKM_KEY_X},           {"Y", VKM_KEY_Y},
    {"Z", VKM_KEY_Z},
    {"Keypad0", VKM_KEY_KEYPAD_0},
    {"Keypad1", VKM_KEY_KEYPAD_1},
    {"Keypad2", VKM_KEY_KEYPAD_2},
    {"Keypad3", VKM_KEY_KEYPAD_3},
    {"Keypad4", VKM_KEY_KEYPAD_4},
    {"Keypad5", VKM_KEY_KEYPAD_5},
    {"Keypad6", VKM_KEY_KEYPAD_6},
    {"Keypad7", VKM_KEY_KEYPAD_7},
    {"Keypad8", VKM_KEY_KEYPAD_8},
    {"Keypad9", VKM_KEY_KEYPAD_9},
    {"KeypadComma", VKM_KEY_KEYPAD_COMMA},
    {"KeypadDecimal", VKM_KEY_KEYPAD_DECIMAL},
    {"KeypadDivide", VKM_KEY_KEYPAD_DIVIDE},
    {"KeypadEnter", VKM_KEY_KEYPAD_ENTER},
    {"KeypadMinus", VKM_KEY_KEYPAD_MINUS},
    {"KeypadMultiply", VKM_KEY_KEYPAD_MULTIPLY},
    {"KeypadNumLock", VKM_KEY_KEYPAD_NUM_LOCK},
    {"KeypadPlus", VKM_KEY_KEYPAD_PLUS},
};

static int parse_key_token(const char *value, vkm_key *out_key)
{
    char tmp[96];
    char *endptr;
    unsigned long u;
    size_t i;
    size_t n;

    if (value == NULL || out_key == NULL) {
        return EINVAL;
    }
    if (strlen(value) >= sizeof(tmp)) {
        return EINVAL;
    }
    memcpy(tmp, value, strlen(value) + 1U);
    trim_inplace(tmp);

    if (tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X')) {
        errno = 0;
        u = strtoul(tmp, &endptr, 16);
        if (errno != 0 || endptr == tmp) {
            return EINVAL;
        }
        while (*endptr != '\0' && isspace((unsigned char)*endptr)) {
            endptr++;
        }
        if (*endptr != '\0' || u >= (unsigned long)VKM_KEY_MAX) {
            return EINVAL;
        }
        *out_key = (vkm_key)u;
        return 0;
    }

    n = sizeof(vkm_key_names) / sizeof(vkm_key_names[0]);
    for (i = 0; i < n; i++) {
        if (!strcmp(tmp, vkm_key_names[i].name)) {
            *out_key = vkm_key_names[i].key;
            return 0;
        }
    }
    return EINVAL;
}

static int token_to_modifier(const char *tok, vkm_key_modifiers *acc)
{
    if (!strcasecmp(tok, "Shift") || !strcasecmp(tok, "LeftShift")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_SHIFT);
    } else if (!strcasecmp(tok, "RightShift")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_RSHIFT);
    } else if (!strcasecmp(tok, "Ctrl") || !strcasecmp(tok, "Control") ||
               !strcasecmp(tok, "LeftCtrl")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_CTRL);
    } else if (!strcasecmp(tok, "RightCtrl")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_RCTRL);
    } else if (!strcasecmp(tok, "Alt") || !strcasecmp(tok, "LeftAlt")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_ALT);
    } else if (!strcasecmp(tok, "RightAlt")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_RALT);
    } else if (!strcasecmp(tok, "Gui") || !strcasecmp(tok, "LeftGui") ||
               !strcasecmp(tok, "Super") || !strcasecmp(tok, "Meta")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_GUI);
    } else if (!strcasecmp(tok, "RightGui")) {
        *acc = (vkm_key_modifiers)(*acc | VKM_KEY_MOD_RGUI);
    } else if (tok[0] == '\0') {
        return 0;
    } else {
        return EINVAL;
    }
    return 0;
}

static int parse_modifiers(const char *value, vkm_key_modifiers *out_mods)
{
    char buf[128];
    char *p;
    char *saveptr = NULL;
    char *tok;

    if (value == NULL || out_mods == NULL) {
        return EINVAL;
    }
    *out_mods = VKM_KEY_MOD_NONE;
    if (value[0] == '\0') {
        return 0;
    }
    if (strlen(value) >= sizeof(buf)) {
        return EINVAL;
    }
    memcpy(buf, value, strlen(value) + 1U);
    trim_inplace(buf);

    /* Split on comma first */
    for (p = buf;; p = NULL) {
        char *seg = strtok_r(p, ",", &saveptr);
        if (seg == NULL) {
            break;
        }
        trim_inplace(seg);
        if (seg[0] == '\0') {
            continue;
        }
        /* Then '+' within each segment */
        char *subsave = NULL;
        char *q;
        for (q = seg;; q = NULL) {
            tok = strtok_r(q, "+", &subsave);
            if (tok == NULL) {
                break;
            }
            trim_inplace(tok);
            if (token_to_modifier(tok, out_mods) != 0) {
                return EINVAL;
            }
        }
    }
    return 0;
}

static int layout_entry_replace_or_append(struct x52d_layout *layout, uint32_t cp, vkm_key key,
                                          vkm_key_modifiers mods)
{
    size_t i;

    for (i = 0; i < layout->n_entries; i++) {
        if (layout->entries[i].cp == cp) {
            layout->entries[i].key = key;
            layout->entries[i].mods = mods;
            return 0;
        }
    }
    {
        struct x52d_layout_entry *ne =
            realloc(layout->entries, (layout->n_entries + 1U) * sizeof(*ne));
        if (ne == NULL) {
            return ENOMEM;
        }
        layout->entries = ne;
        layout->entries[layout->n_entries].cp = cp;
        layout->entries[layout->n_entries].key = key;
        layout->entries[layout->n_entries].mods = mods;
        layout->n_entries++;
    }
    return 0;
}

struct layout_parse_ctx {
    struct x52d_layout *layout;
    char current_section[128];
    bool in_layout_meta;
    bool mapping_have_key;
    uint32_t mapping_cp;
    vkm_key mapping_key;
    vkm_key_modifiers mapping_mods;
    int error;
};

static void layout_flush_mapping(struct layout_parse_ctx *ctx)
{
    int rc;

    if (!ctx->in_layout_meta && ctx->mapping_have_key && ctx->error == 0) {
        rc = layout_entry_replace_or_append(ctx->layout, ctx->mapping_cp, ctx->mapping_key,
                                            ctx->mapping_mods);
        if (rc != 0) {
            ctx->error = rc;
        }
    }
    ctx->mapping_have_key = false;
    ctx->mapping_mods = VKM_KEY_MOD_NONE;
}

static int layout_begin_section(struct layout_parse_ctx *ctx, const char *section)
{
    uint32_t cp;
    size_t consumed;
    int rc;
    size_t slen;

    if (section == NULL) {
        return EINVAL;
    }
    if (strlen(section) >= sizeof(ctx->current_section)) {
        PINELOG_ERROR(_("Layout section name too long"));
        return EINVAL;
    }
    memcpy(ctx->current_section, section, strlen(section) + 1U);

    if (!strcasecmp(section, "Layout")) {
        ctx->in_layout_meta = true;
        return 0;
    }

    ctx->in_layout_meta = false;

    /* INI section names cannot express ']' as a lone code point (e.g. "[]]" is
     * parsed as an empty section). Allow U+ABCD or u+ABCD (1-6 hex digits). */
    if (section[0] == 'U' || section[0] == 'u') {
        if (section[1] == '+' && section[2] != '\0') {
            const char *p = section + 2;
            unsigned long uh = 0;
            char *endp = NULL;

            errno = 0;
            uh = strtoul(p, &endp, 16);
            if (errno == 0 && endp != p && *endp == '\0' && uh <= 0x10FFFFUL &&
                (uh < 0xD800UL || uh > 0xDFFFUL)) {
                ctx->mapping_cp = (uint32_t)uh;
                ctx->mapping_mods = VKM_KEY_MOD_NONE;
                ctx->mapping_have_key = false;
                return 0;
            }
        }
    }

    rc = utf8_decode_one(section, &cp, &consumed);
    if (rc != 0) {
        PINELOG_ERROR(_("Invalid UTF-8 in layout section '%s'"), section);
        return EINVAL;
    }
    slen = strlen(section);
    if (consumed != slen) {
        PINELOG_ERROR(_("Layout section must encode exactly one code point: '%s'"), section);
        return EINVAL;
    }
    ctx->mapping_cp = cp;
    ctx->mapping_mods = VKM_KEY_MOD_NONE;
    ctx->mapping_have_key = false;
    return 0;
}

static int layout_ini_handler(void *user, const char *section, const char *name, const char *value)
{
    struct layout_parse_ctx *ctx = user;
    int rc;
    vkm_key pk;
    vkm_key_modifiers pm;

    if (ctx->error != 0) {
        return 0;
    }
    if (section == NULL || name == NULL || value == NULL) {
        ctx->error = EINVAL;
        return 0;
    }

    if (strcmp(section, ctx->current_section) != 0) {
        layout_flush_mapping(ctx);
        rc = layout_begin_section(ctx, section);
        if (rc != 0) {
            ctx->error = rc;
            return 0;
        }
    }

    if (ctx->in_layout_meta) {
        if (!strcasecmp(name, "Name")) {
            free(ctx->layout->name);
            ctx->layout->name = strdup(value);
            if (ctx->layout->name == NULL) {
                ctx->error = ENOMEM;
            }
        } else if (!strcasecmp(name, "Description")) {
            free(ctx->layout->description);
            ctx->layout->description = strdup(value);
            if (ctx->layout->description == NULL) {
                ctx->error = ENOMEM;
            }
        }
        return ctx->error != 0 ? 0 : 1;
    }

    if (!strcasecmp(name, "Key")) {
        rc = parse_key_token(value, &pk);
        if (rc != 0) {
            PINELOG_ERROR(_("Unknown Key value '%s'"), value);
            ctx->error = EINVAL;
            return 0;
        }
        ctx->mapping_key = pk;
        ctx->mapping_have_key = true;
    } else if (!strcasecmp(name, "Mods")) {
        rc = parse_modifiers(value, &pm);
        if (rc != 0) {
            PINELOG_ERROR(_("Invalid Mods value '%s'"), value);
            ctx->error = EINVAL;
            return 0;
        }
        ctx->mapping_mods = pm;
    }

    /* unknown keys ignored for forward-compatible layout files */
    return ctx->error != 0 ? 0 : 1;
}

static int layout_finalize(struct layout_parse_ctx *ctx)
{
    layout_flush_mapping(ctx);
    if (ctx->error != 0) {
        return ctx->error;
    }
    if (ctx->layout->n_entries > 0U) {
        qsort(ctx->layout->entries, ctx->layout->n_entries, sizeof(ctx->layout->entries[0]),
              cmp_entry_cp);
    }
    return 0;
}

static struct x52d_layout *layout_new(void)
{
    struct x52d_layout *layout = calloc(1, sizeof(*layout));
    return layout;
}

int x52d_layout_load_buffer(struct x52d_layout **out, const char *data, size_t len)
{
    struct layout_parse_ctx ctx;
    char *nulbuf;
    int ir;
    int rc;

    if (out == NULL || data == NULL) {
        return EINVAL;
    }
    *out = NULL;

    ctx.layout = layout_new();
    if (ctx.layout == NULL) {
        return ENOMEM;
    }
    ctx.current_section[0] = '\0';
    ctx.in_layout_meta = false;
    ctx.mapping_have_key = false;
    ctx.mapping_cp = 0;
    ctx.mapping_key = VKM_KEY_NONE;
    ctx.mapping_mods = VKM_KEY_MOD_NONE;
    ctx.error = 0;

    nulbuf = malloc(len + 1U);
    if (nulbuf == NULL) {
        x52d_layout_free(ctx.layout);
        return ENOMEM;
    }
    memcpy(nulbuf, data, len);
    nulbuf[len] = '\0';
    ir = ini_parse_string(nulbuf, layout_ini_handler, &ctx);
    free(nulbuf);

    if (ir < 0) {
        x52d_layout_free(ctx.layout);
        return EIO;
    }
    if (ir > 0) {
        PINELOG_ERROR(_("Layout parse error at line %d"), ir);
        x52d_layout_free(ctx.layout);
        return EINVAL;
    }

    rc = layout_finalize(&ctx);
    if (rc != 0) {
        x52d_layout_free(ctx.layout);
        return rc;
    }

    *out = ctx.layout;
    return 0;
}

int x52d_layout_load_file(struct x52d_layout **out, const char *path)
{
    struct layout_parse_ctx ctx;
    int ir;
    int rc;

    if (out == NULL || path == NULL) {
        return EINVAL;
    }
    *out = NULL;

    ctx.layout = layout_new();
    if (ctx.layout == NULL) {
        return ENOMEM;
    }
    ctx.current_section[0] = '\0';
    ctx.in_layout_meta = false;
    ctx.mapping_have_key = false;
    ctx.mapping_cp = 0;
    ctx.mapping_key = VKM_KEY_NONE;
    ctx.mapping_mods = VKM_KEY_MOD_NONE;
    ctx.error = 0;

    ir = ini_parse(path, layout_ini_handler, &ctx);
    if (ir < 0) {
        x52d_layout_free(ctx.layout);
        return EIO;
    }
    if (ir > 0) {
        PINELOG_ERROR(_("Layout parse error in %s at line %d"), path, ir);
        x52d_layout_free(ctx.layout);
        return EINVAL;
    }

    rc = layout_finalize(&ctx);
    if (rc != 0) {
        x52d_layout_free(ctx.layout);
        return rc;
    }

    *out = ctx.layout;
    return 0;
}
