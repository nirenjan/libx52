/*
 * Saitek X52 Pro MFD & LED driver - Configuration dumper
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "pinelog.h"
#include "libx52.h"
#include "x52d_config.h"
#include "x52d_const.h"

static char *current_section = NULL;

// Print the current section to the file
static void print_section(FILE *cfg, const char *section)
{
    if (current_section == NULL || strcasecmp(current_section, section)) {
        if (current_section != NULL) {
            free(current_section);
        }

        current_section = strdup(section);
        PINELOG_TRACE("Printing section header %s", section);

        fprintf(cfg, "[%s]\n", section);
    }
}

// Create a pointer "name" of type "type", which stores the value of the
// corresponding element within the config struct.
#define CONFIG_PTR(type, name) type name = (type)((uintptr_t)cfg + offset)

// Check if the parameters are all valid
#define CHECK_PARAMS() do { if (cfg == NULL || section == NULL || key == NULL) { return EINVAL; } } while(0)

static int bool_dumper(FILE *file, const char *section, const char *key, const struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(bool *, config);
    CHECK_PARAMS();

    print_section(file, section);
    PINELOG_TRACE("Printing bool value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    fprintf(file, "%s = %s\n", key, *config ? "true" : "false");

    return 0;
}

static int string_dumper(FILE *file, const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(char *, config);
    CHECK_PARAMS();

    print_section(file, section);
    PINELOG_TRACE("Printing string value %s.%s from offset %lu value = %s",
                  section, key, offset, config);
    fprintf(file, "%s = %s\n", key, config);

    return 0;
}

static int int_dumper(FILE *file, const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(int *, config);
    CHECK_PARAMS();

    print_section(file, section);
    PINELOG_TRACE("Printing int value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    fprintf(file, "%s = %d\n", key, *config);

    return 0;
}

static int led_dumper(FILE *file, const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(libx52_led_state *, config);
    CHECK_PARAMS();

    print_section(file, section);
    PINELOG_TRACE("Printing led value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    fprintf(file, "%s = %s\n", key, libx52_led_state_to_str(*config));

    return 0;
}

static int clock_format_dumper(FILE *file, const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(libx52_clock_format *, config);
    CHECK_PARAMS();

    print_section(file, section);
    PINELOG_TRACE("Printing clock format value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    fprintf(file, "%s = %s\n", key, libx52_clock_format_to_str(*config));

    return 0;
}

static int date_format_dumper(FILE *file, const char *section, const char *key, struct x52d_config *cfg, size_t offset)
{
    CONFIG_PTR(libx52_date_format *, config);
    CHECK_PARAMS();

    print_section(file, section);
    PINELOG_TRACE("Printing date format value %s.%s from offset %lu value = %d",
                  section, key, offset, *config);
    fprintf(file, "%s = %s\n", key, libx52_date_format_to_str(*config));

    return 0;
}

#undef CHECK_PARAMS
#undef CONFIG_PTR

int x52d_config_save_file(struct x52d_config *cfg, const char *cfg_file)
{
    int rc;
    FILE *cfg_fp;
    if (cfg == NULL || cfg_file == NULL) {
        return EINVAL;
    }

    if (current_section) {
        free(current_section);
    }
    current_section = NULL;

    cfg_fp = fopen(cfg_file, "w");
    if (cfg_fp == NULL) {
        rc = errno;
        PINELOG_ERROR(_("Unable to save config file %s - code %d: %s"),
                      cfg_file, rc, strerror(rc));
        return rc;
    }

    PINELOG_TRACE("Saving configuration to file %s", cfg_file);
    #define CFG(section, key, name, type, def) do { \
        PINELOG_TRACE("Dumping " #section "." #key " to file %s", cfg_file); \
        rc = type ## _dumper(cfg_fp, #section, #key, cfg, offsetof(struct x52d_config, name)); \
        if (rc) { \
            PINELOG_ERROR(_("Failed to dump %s.%s to config file %s - code %d: %s"), \
                          #section, #key, cfg_file, rc, strerror(rc)); \
            goto exit_dump; \
        } \
    } while (0);
    #include "x52d_config.def"

exit_dump:
    fclose(cfg_fp);
    return rc;
}

