/*
 * Saitek X52 Pro MFD & LED driver - Configuration parser
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PINELOG_MODULE X52D_MOD_CONFIG
#include "pinelog.h"
#include <daemon/config.h>
#include <daemon/constants.h>

static struct x52d_config x52d_config;
static const char *ipc_save_path;

void x52d_config_set_ipc_save_path(const char *path)
{
    ipc_save_path = path;
}

int x52d_config_save_session(void)
{
    int rc;

    if (ipc_save_path != NULL) {
        rc = x52d_config_save_file(&x52d_config, ipc_save_path);
        if (rc != 0) {
            PINELOG_ERROR(_("Error %d saving configuration file: %s"),
                rc, strerror(rc));
        }
        return rc;
    }

    return x52d_config_save_state_atomic();
}

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
void x52d_cfg_set_Profiles_ClutchEnabled(bool param) { (void)param; }
void x52d_cfg_set_Profiles_ClutchLatched(bool param) { (void)param; }

void x52d_config_apply_immediate(const char *section, const char *key)
{
#define CFG(c_sec, c_key, name, parser, def) \
    if (!strcasecmp(section, #c_sec) && !strcasecmp(key, #c_key)) { \
        PINELOG_TRACE("Invoking " #c_sec "." #c_key " callback"); \
        x52d_cfg_set_ ## c_sec ## _ ## c_key(x52d_config . name); \
    } else

#include <daemon/config.def>
    // Dummy to capture the trailing else
    // Wrap it in braces in case tracing has been disabled
    { PINELOG_TRACE("Ignoring apply_immediate(%s.%s)", section, key); }
}

void x52d_config_apply(void)
{
    #define CFG(section, key, name, parser, def) \
        PINELOG_TRACE("Calling configuration callback for " #section "." #key); \
        x52d_cfg_set_ ## section ## _ ## key(x52d_config . name);
    #include <daemon/config.def>
}

static int mkdir_p(char *path)
{
    char *p;
    int saved_errno;

    for (p = path + 1; *p != '\0'; p++) {
        if (*p != '/') {
            continue;
        }
        *p = '\0';
        if (mkdir(path, 0755) != 0 && errno != EEXIST) {
            saved_errno = errno;
            *p = '/';
            return saved_errno != 0 ? saved_errno : EIO;
        }
        *p = '/';
    }
    if (mkdir(path, 0755) != 0 && errno != EEXIST) {
        return errno != 0 ? errno : EIO;
    }
    return 0;
}

static int ensure_dir_for_file(const char *filepath)
{
    char buf[PATH_MAX];
    char *slash;

    if (filepath == NULL || filepath[0] == '\0') {
        return EINVAL;
    }
    if (strlen(filepath) >= sizeof(buf)) {
        return ENAMETOOLONG;
    }
    memcpy(buf, filepath, strlen(filepath) + 1);
    slash = strrchr(buf, '/');
    if (slash == NULL || slash == buf) {
        return 0;
    }
    *slash = '\0';
    return mkdir_p(buf);
}

int x52d_config_reload_canonical(void)
{
    int rc;
    const char *chosen = NULL;

    rc = x52d_config_set_defaults(&x52d_config);
    if (rc != 0) {
        return rc;
    }

    if (access(X52D_STATE_CFG_FILE, R_OK) == 0) {
        chosen = X52D_STATE_CFG_FILE;
    } else if (access(X52D_SYS_CFG_FILE, R_OK) == 0) {
        chosen = X52D_SYS_CFG_FILE;
    }

    if (chosen != NULL) {
        rc = x52d_config_load_file(&x52d_config, chosen);
        if (rc != 0) {
            return rc;
        }
    }

    rc = x52d_config_apply_overrides(&x52d_config);
    x52d_config_clear_overrides();
    return rc;
}

int x52d_config_load_from_path(const char *path)
{
    int rc;

    if (path == NULL || path[0] == '\0') {
        return EINVAL;
    }
    if (access(path, R_OK) != 0) {
        return errno != 0 ? errno : EACCES;
    }

    rc = x52d_config_set_defaults(&x52d_config);
    if (rc != 0) {
        return rc;
    }

    rc = x52d_config_load_file(&x52d_config, path);
    if (rc != 0) {
        return rc;
    }

    rc = x52d_config_apply_overrides(&x52d_config);
    x52d_config_clear_overrides();
    return rc;
}

int x52d_config_reset_to_defaults(void)
{
    int rc;

    rc = x52d_config_set_defaults(&x52d_config);
    if (rc != 0) {
        return rc;
    }
    rc = x52d_config_apply_overrides(&x52d_config);
    x52d_config_clear_overrides();
    return rc;
}

int x52d_config_dump_to_alloc(char **out, size_t *out_len)
{
    FILE *fp;
    int rc;

    if (out == NULL || out_len == NULL) {
        return EINVAL;
    }
    *out = NULL;
    *out_len = 0;

    fp = open_memstream(out, out_len);
    if (fp == NULL) {
        return errno != 0 ? errno : ENOMEM;
    }

    rc = x52d_config_write_ini(&x52d_config, fp, "(memory)");
    if (fclose(fp) != 0) {
        if (rc == 0) {
            rc = errno != 0 ? errno : EIO;
        }
        free(*out);
        *out = NULL;
        *out_len = 0;
        return rc;
    }
    return rc;
}

int x52d_config_save_state_atomic(void)
{
    char template[PATH_MAX];
    int fd;
    FILE *fp;
    int rc;
    int errsv;

    rc = ensure_dir_for_file(X52D_STATE_CFG_FILE);
    if (rc != 0) {
        return rc;
    }

    if (snprintf(template, sizeof template, "%s.XXXXXX", X52D_STATE_CFG_FILE) >= (int)sizeof(template)) {
        return ENAMETOOLONG;
    }

    fd = mkstemp(template);
    if (fd < 0) {
        return errno != 0 ? errno : EIO;
    }

    fp = fdopen(fd, "w");
    if (fp == NULL) {
        errsv = errno;
        close(fd);
        unlink(template);
        return errsv != 0 ? errsv : EIO;
    }

    rc = x52d_config_write_ini(&x52d_config, fp, template);
    if (fflush(fp) != 0 && rc == 0) {
        rc = errno != 0 ? errno : EIO;
    }
    if (fsync(fileno(fp)) != 0 && rc == 0) {
        rc = errno != 0 ? errno : EIO;
    }
    if (fclose(fp) != 0 && rc == 0) {
        rc = errno != 0 ? errno : EIO;
    }
    if (rc != 0) {
        unlink(template);
        return rc;
    }

    if (rename(template, X52D_STATE_CFG_FILE) != 0) {
        errsv = errno;
        unlink(template);
        return errsv != 0 ? errsv : EIO;
    }

    return 0;
}

int x52d_config_clear_disk_then_reload(uint16_t target, int *out_unlink_errno)
{
    const char *path = NULL;
    int reload_rc;

    if (out_unlink_errno != NULL) {
        *out_unlink_errno = 0;
    }

    if (target == X52D_CONFIG_CLEAR_TARGET_STATE) {
        path = X52D_STATE_CFG_FILE;
    } else if (target == X52D_CONFIG_CLEAR_TARGET_SYSCONF) {
        path = X52D_SYS_CFG_FILE;
    } else {
        return EINVAL;
    }

    if (unlink(path) != 0 && errno != ENOENT) {
        if (out_unlink_errno != NULL) {
            *out_unlink_errno = errno != 0 ? errno : EIO;
        }
    }

    reload_rc = x52d_config_reload_canonical();
    return reload_rc;
}
