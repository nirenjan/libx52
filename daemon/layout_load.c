/*
 * Saitek X52 Pro MFD & LED driver - x52layout v1 loader
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"

#include <daemon/crc32.h>
#include <daemon/layout_format.h>
#include <daemon/layout_usage_allowlist.h>

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct x52_layout {
    uint8_t *data;
    size_t size;
    uint32_t codepoint_limit;
    uint16_t flags;
    char *name;
    char *description;
};

static size_t meta_field_len(const uint8_t *field, size_t nbytes)
{
    size_t i;

    for (i = 0; i < nbytes; i++) {
        if (field[i] == '\0') {
            break;
        }
    }
    return i;
}

static char *copy_meta_string(const uint8_t *field, size_t nbytes, bool add_trunc_suffix)
{
    static const char suf[] = "<truncated>";
    const size_t suf_len = sizeof(suf) - 1u;
    size_t base_len = meta_field_len(field, nbytes);
    size_t total = base_len + (add_trunc_suffix ? suf_len : 0);
    char *s = (char *)malloc(total + 1u);

    if (s == NULL) {
        return NULL;
    }
    if (base_len != 0) {
        memcpy(s, field, base_len);
    }
    if (add_trunc_suffix) {
        memcpy(s + base_len, suf, suf_len);
    }
    s[total] = '\0';
    return s;
}

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] << 8 | (uint16_t)p[1]);
}

static uint32_t read_be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | (uint32_t)p[3];
}

static uint32_t x52_layout_file_crc(const uint8_t *buf, size_t len)
{
    uint32_t crc = x52_crc32_init();
    crc = x52_crc32_update(crc, buf, 12u);
    static const uint8_t zero_chk[4] = {0, 0, 0, 0};
    crc = x52_crc32_update(crc, zero_chk, 4u);
    if (len > 16u) {
        crc = x52_crc32_update(crc, buf + 16u, len - 16u);
    }
    return crc;
}

static int validate_entries(const uint8_t *buf, uint32_t codepoint_limit)
{
    const uint8_t *base = buf + X52_LAYOUT_HEADER_BYTES;

    for (uint32_t i = 0; i < codepoint_limit; i++) {
        uint8_t usage = base[2u * (size_t)i + 1u];
        if (usage == 0) {
            continue;
        }
        if (!x52_layout_usage_key_allowed(usage)) {
            return EINVAL;
        }
    }
    return 0;
}

static int layout_validate_and_adopt(uint8_t *buf, size_t len, x52_layout **out)
{
    if (out == NULL) {
        free(buf);
        return EINVAL;
    }
    *out = NULL;

    if (len < X52_LAYOUT_HEADER_BYTES) {
        free(buf);
        return EINVAL;
    }

    if (memcmp(buf, "X52L", 4) != 0) {
        free(buf);
        return EINVAL;
    }

    uint16_t version = read_be16(buf + 4);
    uint16_t flags = read_be16(buf + 6);
    uint32_t codepoint_limit = read_be32(buf + 8);
    uint32_t stored_crc = read_be32(buf + 12);

    if (version != X52_LAYOUT_FORMAT_VERSION) {
        free(buf);
        return EINVAL;
    }
    if ((flags & (uint16_t)~X52_LAYOUT_FLAGS_KNOWN) != 0) {
        free(buf);
        return EINVAL;
    }
    if (codepoint_limit > X52_LAYOUT_CODEPOINT_LIMIT_MAX) {
        free(buf);
        return EINVAL;
    }

    size_t body = (size_t)codepoint_limit * 2u;
    size_t expected = X52_LAYOUT_HEADER_BYTES + body;
    if (expected < X52_LAYOUT_HEADER_BYTES || len != expected) {
        free(buf);
        return EINVAL;
    }

    if (meta_field_len(buf + 16, X52_LAYOUT_NAME_FIELD_BYTES) == 0) {
        free(buf);
        return EINVAL;
    }

    uint32_t calc = x52_layout_file_crc(buf, len);
    if (calc != stored_crc) {
        free(buf);
        return EINVAL;
    }

    int en = validate_entries(buf, codepoint_limit);
    if (en != 0) {
        free(buf);
        return en;
    }

    x52_layout *L = (x52_layout *)malloc(sizeof *L);
    if (L == NULL) {
        free(buf);
        return ENOMEM;
    }
    L->data = buf;
    L->size = len;
    L->codepoint_limit = codepoint_limit;
    L->flags = flags;
    L->name = copy_meta_string(buf + 16, X52_LAYOUT_NAME_FIELD_BYTES,
                               (flags & X52_LAYOUT_FLAG_NAME_TRUNCATED) != 0);
    if (L->name == NULL) {
        free(L);
        free(buf);
        return ENOMEM;
    }
    L->description = copy_meta_string(buf + 48, X52_LAYOUT_DESCRIPTION_FIELD_BYTES,
                                     (flags & X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED) != 0);
    if (L->description == NULL) {
        free(L->name);
        free(L);
        free(buf);
        return ENOMEM;
    }
    *out = L;
    return 0;
}

void x52_layout_normalize_keyboard_basename(const char *cfg_value, char *out, size_t out_sz, bool *rejected_out)
{
    *rejected_out = false;
    if (out_sz < 3) {
        if (out_sz > 0) {
            out[0] = '\0';
        }
        return;
    }

    if (cfg_value == NULL || cfg_value[0] == '\0') {
        out[0] = 'u';
        out[1] = 's';
        out[2] = '\0';
        return;
    }

    if (strchr(cfg_value, '/') != NULL || strchr(cfg_value, '\\') != NULL ||
        strstr(cfg_value, "..") != NULL) {
        goto failed_normalization;
    }

    size_t len = strlen(cfg_value);
    if (len >= out_sz) {
        goto failed_normalization;
    }

    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)cfg_value[i];
        if (!isalnum((int)c) && c != '_' && c != '-') {
            goto failed_normalization;
        }
    }

    memcpy(out, cfg_value, len + 1u);
    return;

failed_normalization:
    *rejected_out = true;
    out[0] = 'u';
    out[1] = 's';
    out[2] = '\0';
}

int x52_layout_join_file_path(char *path, size_t path_sz, const char *datadir, const char *basename)
{
    if (path == NULL || datadir == NULL || basename == NULL || path_sz == 0) {
        return EINVAL;
    }

    int n = snprintf(path, path_sz, "%s/x52d/%s.x52l", datadir, basename);
    if (n < 0) {
        return EIO;
    }
    if ((size_t)n >= path_sz) {
        return ENAMETOOLONG;
    }
    return 0;
}

int x52_layout_load_datadir(const char *datadir, const char *basename, x52_layout **out)
{
    char path[PATH_MAX];

    int rc = x52_layout_join_file_path(path, sizeof path, datadir, basename);
    if (rc != 0) {
        return rc;
    }
    return x52_layout_load_path(path, out);
}

int x52_layout_load_path(const char *path, x52_layout **out)
{
    if (path == NULL) {
        return EINVAL;
    }

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return errno != 0 ? errno : EIO;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        int e = errno != 0 ? errno : EIO;
        fclose(fp);
        return e;
    }

    long pos = ftell(fp);
    if (pos < 0) {
        int e = errno != 0 ? errno : EIO;
        fclose(fp);
        return e;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        int e = errno != 0 ? errno : EIO;
        fclose(fp);
        return e;
    }

    size_t len = (size_t)pos;
    if ((long)len != pos || pos < (long)X52_LAYOUT_HEADER_BYTES) {
        fclose(fp);
        return EINVAL;
    }

    uint8_t *buf = (uint8_t *)malloc(len);
    if (buf == NULL) {
        fclose(fp);
        return ENOMEM;
    }

    size_t n = fread(buf, 1, len, fp);
    fclose(fp);
    if (n != len) {
        free(buf);
        return EIO;
    }

    return layout_validate_and_adopt(buf, len, out);
}

int x52_layout_load_memory(const void *data, size_t len, x52_layout **out)
{
    if (data == NULL && len != 0) {
        return EINVAL;
    }

    uint8_t *buf = (uint8_t *)malloc(len);
    if (buf == NULL) {
        return ENOMEM;
    }
    if (len != 0) {
        memcpy(buf, data, len);
    }
    return layout_validate_and_adopt(buf, len, out);
}

void x52_layout_free(x52_layout *layout)
{
    if (layout == NULL) {
        return;
    }
    free(layout->name);
    free(layout->description);
    free(layout->data);
    free(layout);
}

uint32_t x52_layout_codepoint_limit(const x52_layout *layout)
{
    if (layout == NULL) {
        return 0;
    }
    return layout->codepoint_limit;
}

uint16_t x52_layout_flags(const x52_layout *layout)
{
    if (layout == NULL) {
        return 0;
    }
    return layout->flags;
}

const char *x52_layout_name(const x52_layout *layout)
{
    if (layout == NULL || layout->name == NULL) {
        return "";
    }
    return layout->name;
}

const char *x52_layout_description(const x52_layout *layout)
{
    if (layout == NULL || layout->description == NULL) {
        return "";
    }
    return layout->description;
}

bool x52_layout_lookup(const x52_layout *layout, uint32_t code_point, uint8_t *modifiers_out,
                       uint8_t *usage_key_out)
{
    if (layout == NULL || modifiers_out == NULL || usage_key_out == NULL) {
        return false;
    }
    if (code_point >= layout->codepoint_limit) {
        return false;
    }

    size_t off = X52_LAYOUT_HEADER_BYTES + 2u * (size_t)code_point;
    uint8_t mod = layout->data[off];
    uint8_t usage = layout->data[off + 1u];
    if (usage == 0) {
        return false;
    }
    *modifiers_out = mod;
    *usage_key_out = usage;
    return true;
}
