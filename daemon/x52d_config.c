/*
 * Saitek X52 Pro MFD & LED driver - Configuration parser
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <errno.h>

#define PINELOG_MODULE X52D_MOD_CONFIG
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"

static struct x52d_config x52d_config;

void x52d_config_load(const char *cfg_file)
{
    int rc;

    if (cfg_file == NULL) {
        cfg_file = X52D_SYS_CFG_FILE;
    }

    rc = x52d_config_set_defaults(&x52d_config);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d setting configuration defaults: %s"),
                      rc, strerror(rc));
    }

    rc = x52d_config_load_file(&x52d_config, cfg_file);
    if (rc != 0) {
        exit(EXIT_FAILURE);
    }

    // Apply overrides
    rc = x52d_config_apply_overrides(&x52d_config);
    x52d_config_clear_overrides();
    if (rc != 0) {
        exit(EXIT_FAILURE);
    }
}

void x52d_config_save(const char *cfg_file)
{
    int rc;

    if (cfg_file == NULL) {
        cfg_file = X52D_SYS_CFG_FILE;
    }

    rc = x52d_config_save_file(&x52d_config, cfg_file);
    if (rc != 0) {
        PINELOG_ERROR(_("Error %d saving configuration file: %s"),
                      rc, strerror(rc));
    }
}

int x52d_config_set(const char *section, const char *key, const char *value)
{
    if (section == NULL || key == NULL || value == NULL) {
        return EINVAL;
    }

    PINELOG_TRACE("Processing config set '%s.%s'='%s'", section, key, value);

    return x52d_config_process_kv(&x52d_config, section, key, value);
}

const char *x52d_config_get(const char *section, const char *key)
{
    const char *value;
    if (section == NULL || key == NULL) {
        return NULL;
    }

    value = x52d_config_get_param(&x52d_config, section, key);
    PINELOG_TRACE("Processed config get '%s.%s'='%s'", section, key, value);

    return value;
}

/* Callback stubs
 * TODO: Remove the ones below when their implementation is complete
 */
void x52d_cfg_set_Profiles_Directory(char* param) { (void)param; }
void x52d_cfg_set_Profiles_Profile(char* param) { (void)param; }
void x52d_cfg_set_Profiles_ClutchEnabled(bool param) { (void)param; }
void x52d_cfg_set_Profiles_ClutchLatched(bool param) { (void)param; }

void x52d_config_apply_immediate(const char *section, const char *key)
{
#define CFG(c_sec, c_key, name, parser, def) \
    if (!strcasecmp(section, #c_sec) && !strcasecmp(key, #c_key)) { \
        PINELOG_TRACE("Invoking " #c_sec "." #c_key " callback"); \
        x52d_cfg_set_ ## c_sec ## _ ## c_key(x52d_config . name); \
    } else

#include "x52d_config.def"
    // Dummy to capture the trailing else
    // Wrap it in braces in case tracing has been disabled
    { PINELOG_TRACE("Ignoring apply_immediate(%s.%s)", section, key); }
}

void x52d_config_apply(void)
{
    #define CFG(section, key, name, parser, def) \
        PINELOG_TRACE("Calling configuration callback for " #section "." #key); \
        x52d_cfg_set_ ## section ## _ ## key(x52d_config . name);
    #include "x52d_config.def"
}
