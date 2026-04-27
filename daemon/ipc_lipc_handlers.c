/*
 * Saitek X52 Pro MFD & LED driver - framed IPC opcode handlers
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PINELOG_MODULE X52D_MOD_CLIENT
#include "pinelog.h"
#include <daemon/config.h>
#include <daemon/constants.h>
#include <daemon/ipc_lipc_handlers.h>
#include <localipc/lipc.h>

#include "config-defs.h"
#include "module-map.h"

static lipc_status reply_err(lipc_server_reply_ctx *reply, lipc_status st, const char *msg)
{
    size_t len = (msg != NULL) ? strlen(msg) : 0;

    return lipc_server_reply(reply, (uint16_t)st, 0, 0, msg, len);
}

static lipc_status reply_ok(lipc_server_reply_ctx *reply, const void *payload, size_t payload_len)
{
    return lipc_server_reply(reply, LIPC_OK, 0, 0, payload, payload_len);
}

static lipc_status on_config_load(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    char *path;
    int rc;
    (void)user;
    (void)req_hdr;

    path = malloc(payload_len + 1u);
    if (path == NULL) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, strerror(ENOMEM));
    }
    memcpy(path, payload, payload_len);
    path[payload_len] = '\0';

    rc = x52d_config_load_from_path(path);
    free(path);
    if (rc != 0) {
        return reply_err(reply, LIPC_INVALID_PARAMS, strerror(rc));
    }
    x52d_config_apply();
    return reply_ok(reply, NULL, 0);
}

static lipc_status on_config_reload(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    int rc;
    (void)user;
    (void)req_hdr;
    (void)payload;
    (void)payload_len;

    rc = x52d_config_reload_canonical();
    if (rc != 0) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, strerror(rc));
    }
    x52d_config_apply();
    return reply_ok(reply, NULL, 0);
}

static lipc_status on_config_reset(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    int rc;
    (void)user;
    (void)req_hdr;
    (void)payload;
    (void)payload_len;

    rc = x52d_config_reset_to_defaults();
    if (rc != 0) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, strerror(rc));
    }
    x52d_config_apply();
    return reply_ok(reply, NULL, 0);
}

static lipc_status on_config_clear(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    int rc;
    int unlink_err = 0;
    (void)user;
    (void)payload;
    (void)payload_len;

    if (req_hdr->index != X52D_CONFIG_CLEAR_TARGET_STATE
        && req_hdr->index != X52D_CONFIG_CLEAR_TARGET_SYSCONF) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid CONFIG_CLEAR target index");
    }

    rc = x52d_config_clear_disk_then_reload((uint16_t)req_hdr->index, &unlink_err);
    if (rc != 0) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, strerror(rc));
    }
    x52d_config_apply();
    if (unlink_err != 0) {
        return reply_err(reply, LIPC_IO_ERROR, strerror(unlink_err));
    }
    return reply_ok(reply, NULL, 0);
}

static lipc_status on_config_save(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    int rc;
    (void)user;
    (void)req_hdr;
    (void)payload;
    (void)payload_len;

    rc = x52d_config_save_session();
    if (rc != 0) {
        return reply_err(reply, LIPC_IO_ERROR, strerror(rc));
    }
    return reply_ok(reply, NULL, 0);
}

static lipc_status on_config_dump(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    char *buf = NULL;
    size_t len = 0;
    int rc;
    lipc_status st;
    (void)user;
    (void)req_hdr;
    (void)payload;
    (void)payload_len;

    rc = x52d_config_dump_to_alloc(&buf, &len);
    if (rc != 0) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, strerror(rc));
    }

    st = reply_ok(reply, buf, len);
    free(buf);
    return st;
}

static lipc_status on_config_set(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    const char *sec_name;
    const char *opt_name;
    char *val_copy;
    int rc;
    uint16_t option_id;
    (void)user;

    if (req_hdr->value > UINT16_MAX) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "option id out of range");
    }
    option_id = (uint16_t)req_hdr->value;
    if (!x52d_config_registry_pair_valid(req_hdr->index, option_id)) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid section or option id");
    }

    sec_name = section_names[req_hdr->index];
    opt_name = option_names[req_hdr->index][option_id];
    if (sec_name == NULL || opt_name == NULL) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid configuration key");
    }

    val_copy = malloc(payload_len + 1u);
    if (val_copy == NULL) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, strerror(ENOMEM));
    }
    memcpy(val_copy, payload, payload_len);
    val_copy[payload_len] = '\0';

    rc = x52d_config_set(sec_name, opt_name, val_copy);
    free(val_copy);
    if (rc != 0) {
        return reply_err(reply, LIPC_INVALID_PARAMS, strerror(rc));
    }
    x52d_config_apply_immediate(sec_name, opt_name);
    return reply_ok(reply, NULL, 0);
}

static lipc_status on_config_get(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    const char *sec_name;
    const char *opt_name;
    const char *val;
    uint16_t option_id;
    (void)user;
    (void)payload;
    (void)payload_len;

    if (req_hdr->value > UINT16_MAX) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "option id out of range");
    }
    option_id = (uint16_t)req_hdr->value;
    if (!x52d_config_registry_pair_valid(req_hdr->index, option_id)) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid section or option id");
    }

    sec_name = section_names[req_hdr->index];
    opt_name = option_names[req_hdr->index][option_id];
    val = x52d_config_get(sec_name, opt_name);
    if (val == NULL) {
        return reply_err(reply, LIPC_NOT_FOUND, "configuration key not found");
    }

    return reply_ok(reply, val, strlen(val));
}

static lipc_status on_logging_show(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    int level;
    const char *label;
    (void)user;
    (void)payload;
    (void)payload_len;

    if (!x52d_module_wire_valid(req_hdr->index)) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid module id");
    }

    if (req_hdr->index == X52D_MOD_GLOBAL) {
        level = pinelog_get_level();
    } else {
        level = pinelog_get_module_level((int)req_hdr->index);
    }

    label = lookup_level_by_id(level);
    if (label == NULL) {
        return reply_err(reply, LIPC_INTERNAL_ERROR, "unknown log level in effect");
    }

    return reply_ok(reply, label, strlen(label));
}

static lipc_status on_logging_set(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    int64_t wire = (int64_t)req_hdr->value;
    (void)user;
    (void)payload;
    (void)payload_len;

    if (!x52d_module_wire_valid(req_hdr->index)) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid module id");
    }
    if (!x52d_log_level_wire_valid(req_hdr->value)) {
        return reply_err(reply, LIPC_INVALID_PARAMS, "invalid log level id");
    }

    if (req_hdr->index == X52D_MOD_GLOBAL) {
        pinelog_set_level((int)wire);
    } else {
        pinelog_set_module_level((int)req_hdr->index, (int)wire);
    }

    return reply_ok(reply, NULL, 0);
}

static const lipc_method_desc desc_config_load = {
    .index = LIPC_FIELD_FORBIDDEN,
    .value = LIPC_FIELD_FORBIDDEN,
    .payload = LIPC_FIELD_REQUIRED,
    .payload_min_len = 1,
    .payload_max_len = PATH_MAX - 1,
};

static const lipc_method_desc desc_config_void = {
    .index = LIPC_FIELD_FORBIDDEN,
    .value = LIPC_FIELD_FORBIDDEN,
    .payload = LIPC_FIELD_FORBIDDEN,
    .payload_min_len = 0,
    .payload_max_len = 0,
};

static const lipc_method_desc desc_config_clear = {
    .index = LIPC_FIELD_REQUIRED,
    .value = LIPC_FIELD_FORBIDDEN,
    .payload = LIPC_FIELD_FORBIDDEN,
    .payload_min_len = 0,
    .payload_max_len = 0,
};

static const lipc_method_desc desc_config_dump = {
    .index = LIPC_FIELD_FORBIDDEN,
    .value = LIPC_FIELD_FORBIDDEN,
    .payload = LIPC_FIELD_OPTIONAL,
    .payload_min_len = 0,
    .payload_max_len = UINT32_MAX,
};

static const lipc_method_desc desc_config_set = {
    .index = LIPC_FIELD_REQUIRED,
    .value = LIPC_FIELD_REQUIRED,
    .payload = LIPC_FIELD_REQUIRED,
    .payload_min_len = 1,
    .payload_max_len = NAME_MAX,
};

static const lipc_method_desc desc_config_get = {
    .index = LIPC_FIELD_REQUIRED,
    .value = LIPC_FIELD_REQUIRED,
    .payload = LIPC_FIELD_FORBIDDEN,
    .payload_min_len = 0,
    .payload_max_len = 0,
};

static const lipc_method_desc desc_logging_show = {
    /* Module id 0 is valid (e.g. CONFIG); liblocalipc REQUIRED would incorrectly reject index==0. */
    .index = LIPC_FIELD_OPTIONAL,
    /* SHOW reads level from pinelog only; @c value must be zero on the wire. */
    .value = LIPC_FIELD_FORBIDDEN,
    .payload = LIPC_FIELD_FORBIDDEN,
    .payload_min_len = 0,
    .payload_max_len = 0,
};

static const lipc_method_desc desc_logging_set = {
    .index = LIPC_FIELD_OPTIONAL,
    /* FATAL is level id 0; REQUIRED would reject it — validate with @c x52d_log_level_wire_valid. */
    .value = LIPC_FIELD_OPTIONAL,
    .payload = LIPC_FIELD_FORBIDDEN,
    .payload_min_len = 0,
    .payload_max_len = 0,
};

static const lipc_server_handler x52d_ipc_handlers[] = {
    { X52D_IPC_CONFIG_LOAD, &desc_config_load, on_config_load },
    { X52D_IPC_CONFIG_RELOAD, &desc_config_void, on_config_reload },
    { X52D_IPC_CONFIG_RESET, &desc_config_void, on_config_reset },
    { X52D_IPC_CONFIG_CLEAR, &desc_config_clear, on_config_clear },
    { X52D_IPC_CONFIG_SAVE, &desc_config_void, on_config_save },
    { X52D_IPC_CONFIG_DUMP, &desc_config_dump, on_config_dump },
    { X52D_IPC_CONFIG_SET, &desc_config_set, on_config_set },
    { X52D_IPC_CONFIG_GET, &desc_config_get, on_config_get },
    { X52D_IPC_LOGGING_SHOW, &desc_logging_show, on_logging_show },
    { X52D_IPC_LOGGING_SET, &desc_logging_set, on_logging_set },
};

const lipc_server_handler *x52d_ipc_handlers_table(void)
{
    return x52d_ipc_handlers;
}

size_t x52d_ipc_handlers_count(void)
{
    return sizeof(x52d_ipc_handlers) / sizeof(x52d_ipc_handlers[0]);
}
