/*
 * Saitek X52 Pro MFD & LED driver - Configuration parser
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

#include "ini.h"
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"

/* Parser function typedef */
typedef int (*parser_fn)(struct x52d_config *, size_t, const char *);

// Check if the parameters are all valid
#define CHECK_PARAMS() do { if (cfg == NULL || value == NULL) { return EINVAL; } } while(0)

// Create a pointer "name" of type "type", which stores the pointer to the
// corresponding element within the config struct.
#define CONFIG_PTR(type, name) type name = (type)((uintptr_t)cfg + offset)

static int bool_parser(struct x52d_config *cfg, size_t offset, const char *value)
{
    CONFIG_PTR(bool *, config);
    CHECK_PARAMS();

    if (!strcasecmp(value, "yes") || !strcasecmp(value, "true")) {
        *config = true;
    } else if (!strcasecmp(value, "no") || !strcasecmp(value, "false")) {
        *config = false;
    } else {
        return EINVAL;
    }

    return 0;
}

static int string_parser(struct x52d_config *cfg, size_t offset, const char *value)
{
    CONFIG_PTR(char *, config);
    CHECK_PARAMS();

    /* String parameters are all NAME_MAX len */
    strncpy(config, value, NAME_MAX-1);
    config[NAME_MAX-1] = '\0';

    return 0;
}

static int int_parser(struct x52d_config *cfg, size_t offset, const char *value)
{
    CONFIG_PTR(int *, config);
    char *endptr;
    int retval;

    CHECK_PARAMS();

    errno = 0;
    retval = strtol(value, &endptr, 0);
    if (errno != 0) {
        return errno;
    }

    *config = retval;
    return 0;
}

static int led_parser(struct x52d_config *cfg, size_t offset, const char *value)
{
    CONFIG_PTR(libx52_led_state *, config);
    CHECK_PARAMS();

    #define MATCH_STATE(val) if (!strcasecmp(value, #val)) { *config = LIBX52_LED_STATE_ ## val ;  }
    MATCH_STATE(OFF)
    else MATCH_STATE(ON)
    else MATCH_STATE(RED)
    else MATCH_STATE(AMBER)
    else MATCH_STATE(GREEN)
    else return EINVAL;
    #undef MATCH_STATE

    return 0;
}

/* Map for config->param */
#define CFG(section, key, name, parser, def) {#section, #key, parser, offsetof(struct x52d_config, name)},
const struct config_map {
    const char *section;
    const char *key;
    parser_fn parser;
    size_t offset;
} config_map[] = {
    #include "x52d_config.def"

    // Terminating entry
    {NULL, NULL, NULL, 0}
};

static int process_config_kv(void *user, const char *section, const char *key, const char *value)
{
    int i;
    int rc = 0;
    bool found = false;
    struct x52d_config *cfg = (struct x52d_config*)user;

    for (i = 0; config_map[i].key != NULL; i++) {
        rc = 0;
        if (!strcasecmp(config_map[i].key, key) &&
            !strcasecmp(config_map[i].section, section)) {
            found = true;
            PINELOG_TRACE("Setting '%s.%s'='%s'",
                          config_map[i].section, config_map[i].key, value);
            rc = config_map[i].parser(cfg, config_map[i].offset, value);
            break;
        }
    }

    if (!found) {
        // Print error message, but continue
        PINELOG_INFO(_("Ignoring unknown key '%s.%s'"), section, key);
    }

    return rc;
}

/**
 * @brief Set configuration defaults
 *
 * @param[in]   cfg     Pointer to config struct
 *
 * @returns 0 on success, non-zero error code on failure
 */
int x52d_config_set_defaults(struct x52d_config *cfg) {
    int rc;

    if (cfg == NULL) {
        return EINVAL;
    }

    PINELOG_TRACE("Setting configuration defaults");
    #define CFG(section, key, name, parser, def) \
        rc = process_config_kv(cfg, #section, #key, #def); \
        if (rc != 0) { \
            return rc; \
        }
    #include "x52d_config.def"

    return 0;
}

int x52d_config_load_file(struct x52d_config *cfg, const char *cfg_file)
{
    int rc;
    if (cfg == NULL || cfg_file == NULL) {
        return EINVAL;
    }

    PINELOG_TRACE("Loading configuration from file %s", cfg_file);
    rc = ini_parse(cfg_file, process_config_kv, cfg);
    if (rc < 0) {
        PINELOG_ERROR(_("Failed processing configuration file %s - code %d"),
                      cfg_file, rc);
        return EIO;
    }

    return 0;
}

struct x52d_config_override {
    char *section;
    char *key;
    char *value;
    struct x52d_config_override *next;
};

static struct x52d_config_override *override_head;
static struct x52d_config_override *override_tail;

int x52d_config_save_override(const char *override_str)
{
    // Parse override string of the form section.key=value
    struct x52d_config_override *override;
    char *string = NULL;
    char *free_ptr = NULL;
    char *ptr;
    int rc;

    PINELOG_TRACE("Allocating memory (%lu bytes) for override structure", sizeof(*override));
    override = calloc(1, sizeof(*override));
    if (override == NULL) {
        PINELOG_ERROR(_("Failed to allocate memory for override structure"));
        rc = ENOMEM;
        goto cleanup;
    }

    errno = 0;
    PINELOG_TRACE("Duplicating override string");
    string = strdup(override_str);
    if (string == NULL) {
        PINELOG_ERROR(_("Failed to allocate memory for override string"));
        rc = errno;
        goto cleanup;
    }
    free_ptr = string;

    override->section = string;
    // Ensure that the string is of the form ([^.]+\.[^=]+=.*)
    ptr = strchr(string, '.');
    if (ptr == NULL || ptr == string) {
        // No section found
        PINELOG_ERROR(_("No section found in override string '%s'"), string);
        rc = EINVAL;
        goto cleanup;
    }
    // Reset the . to NUL
    *ptr = '\0';
    ptr++;
    PINELOG_TRACE("Splitting override string to '%s' and '%s'", string, ptr);
    string = ptr;

    override->key = string;
    ptr = strchr(string, '=');
    if (ptr == NULL || ptr == string) {
        // No key found
        PINELOG_ERROR(_("No key found in override string '%s'"), string);
        rc = EINVAL;
        goto cleanup;
    }
    // Reset the = to NUL
    *ptr = '\0';
    ptr++;
    PINELOG_TRACE("Splitting override string to '%s' and '%s'", string, ptr);

    if (*ptr == '\0') {
        // No value found
        PINELOG_ERROR(_("No value found in override string '%s'"), string);
        rc = EINVAL;
        goto cleanup;
    }

    override->value = ptr;

    // Add the override to the linked list
    if (override_tail != NULL) {
        PINELOG_TRACE("Linking override to list tail");
        override_tail->next = override;
    }
    PINELOG_TRACE("Setting list tail to override");
    override_tail = override;

    if (override_head == NULL) {
        PINELOG_TRACE("Setting list head to override");
        override_head = override;
    }

    return 0;

cleanup:
    if (free_ptr != NULL) {
        free(free_ptr);
    }
    if (override != NULL) {
        free(override);
    }
    return rc;
}

int x52d_config_apply_overrides(struct x52d_config *cfg)
{
    int rc;
    struct x52d_config_override *tmp = override_head;

    if (cfg == NULL) {
        return EINVAL;
    }

    while (tmp != NULL) {
        PINELOG_TRACE("Processing override '%s.%s=%s'",
                      tmp->section,
                      tmp->key,
                      tmp->value);
        rc = process_config_kv(cfg,
                               tmp->section,
                               tmp->key,
                               tmp->value);
        if (rc != 0) {
            PINELOG_ERROR(_("Error processing override '%s.%s=%s'"),
                          tmp->section,
                          tmp->key,
                          tmp->value);
            return rc;
        }
        tmp = tmp->next;
    }

    return 0;
}

void x52d_config_clear_overrides(void)
{
    struct x52d_config_override *tmp;
    while (override_head != NULL) {
        tmp = override_head;
        override_head = override_head->next;
        PINELOG_TRACE("Freeing override '%s.%s=%s'",
                      tmp->section,
                      tmp->key,
                      tmp->value);
        free(tmp);
    }

    override_tail = NULL;
}
