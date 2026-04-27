/*
 * Saitek X52 Pro MFD & LED driver - Configuration dumper
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define PINELOG_MODULE X52D_MOD_CONFIG
#include "pinelog.h"
#include <libx52/libx52.h>
#include <daemon/config.h>
#include <daemon/constants.h>

// Create a pointer "name" of type "type", which stores the value of the
// corresponding element within the config struct.
#define CONFIG_PTR(type, name) type name = (type)((uintptr_t)cfg + offset)

// Check if the parameters are all valid
#define CHECK_PARAMS() do { if (cfg == NULL || section == NULL || key == NULL) { return NULL; } } while(0)

static const char * bool_dumper(const char *section, const char *key, const struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(bool *, config);
    CHECK_PARAMS();

    PINELOG_TRACE("Printing bool value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    return *config ? "true" : "false";
}

static const char * string_dumper(const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(char *, config);
    CHECK_PARAMS();

    PINELOG_TRACE("Printing string value %s.%s from offset %lu value = %s",
                  section, key, offset, config);
    return config;
}

static const char * int_dumper(const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    static char dump[256];
    CONFIG_PTR(int *, config);
    CHECK_PARAMS();

    PINELOG_TRACE("Printing int value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    snprintf(dump, sizeof(dump), "%d", *config);

    return dump;
}

static const char * led_dumper(const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(libx52_led_state *, config);
    CHECK_PARAMS();

    PINELOG_TRACE("Printing led value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    return libx52_led_state_to_str(*config);
}

static const char * clock_format_dumper(const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(libx52_clock_format *, config);
    CHECK_PARAMS();

    PINELOG_TRACE("Printing clock format value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    return libx52_clock_format_to_str(*config);
}

static const char * date_format_dumper(const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(libx52_date_format *, config);
    CHECK_PARAMS();

    PINELOG_TRACE("Printing date format value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    return libx52_date_format_to_str(*config);
}

#undef CHECK_PARAMS
#undef CONFIG_PTR

int x52d_config_write_ini(struct x52d_config *cfg, FILE *cfg_fp, const char *path_label)
{
    char *current_section = NULL;
    const char *value = "";
    int out_rc = 0;

    if (cfg == NULL || cfg_fp == NULL || path_label == NULL) {
        return EINVAL;
    }

    PINELOG_TRACE("Writing configuration INI to %s", path_label);
    #define CFG(section, key, name, type, def) do { \
        if (current_section == NULL || strcasecmp(current_section, #section)) { \
            if (current_section != NULL) { \
                free(current_section); \
            } \
            current_section = strdup(#section); \
            if (current_section == NULL) { \
                value = NULL; \
                out_rc = ENOMEM; \
                goto exit_write_ini; \
            } \
            PINELOG_TRACE("Printing section header %s", #section); \
            fprintf(cfg_fp, "[%s]\n", #section); \
        } \
        PINELOG_TRACE("Dumping " #section "." #key " to %s", path_label); \
        value = type ## _dumper(#section, #key, cfg, offsetof(struct x52d_config, name)); \
        if (value == NULL) { \
            PINELOG_ERROR(_("Failed to dump %s.%s to config stream %s"), \
                          #section, #key, path_label); \
            out_rc = EIO; \
            goto exit_write_ini; \
        } \
        fprintf(cfg_fp, "%s = %s\n", #key, value); \
    } while (0);
    #include <daemon/config.def>

exit_write_ini:
    free(current_section);
    return out_rc;
}

int x52d_config_save_file(struct x52d_config *cfg, const char *cfg_file)
{
    FILE *cfg_fp;
    int rc;

    if (cfg == NULL || cfg_file == NULL) {
        return EINVAL;
    }

    cfg_fp = fopen(cfg_file, "w");
    if (cfg_fp == NULL) {
        PINELOG_ERROR(_("Unable to save config file %s - code %d: %s"),
                      cfg_file, errno, strerror(errno));
        return errno != 0 ? errno : EIO;
    }

    rc = x52d_config_write_ini(cfg, cfg_fp, cfg_file);
    if (fclose(cfg_fp) != 0 && rc == 0) {
        rc = errno != 0 ? errno : EIO;
    }
    return rc;
}

const char *x52d_config_get_param(struct x52d_config *cfg, const char *section, const char *key)
{
    #define CFG(section_c, key_c, name, type, def) do { \
        if (strcasecmp(section, #section_c) == 0 && strcasecmp(key, #key_c) == 0) { \
            return type ## _dumper(section, key, cfg, offsetof(struct x52d_config, name)); \
        } \
    } while (0);
    #include <daemon/config.def>

    return NULL;
}
