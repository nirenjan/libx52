/*
 * Saitek X52 Pro MFD & LED driver - framed IPC listener (unified command + notify)
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "build-config.h"
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define PINELOG_MODULE X52D_MOD_CLIENT
#include "pinelog.h"
#include <daemon/constants.h>
#include <daemon/ipc_lipc_handlers.h>
#include <daemon/ipc_service.h>
#include <daemon/x52dcomm-internal.h>
#include <libx52/x52d_ipc.h>
#include <localipc/lipc.h>

static lipc_server *ipc_srv_ctx;
static pthread_t ipc_listen_thread;
static const char *ipc_path_stored;
static bool ipc_listen_thread_started;

static void *ipc_listen_thread_main(void *arg)
{
    lipc_server *srv = (lipc_server *)arg;
    (void)lipc_server_run(srv);
    return NULL;
}

static void ipc_stop_join(pthread_t *thr, bool *started, lipc_server **ctx,
    const char *path_for_unlink)
{
    if (*ctx != NULL) {
        (void)lipc_server_stop(*ctx);
    }
    if (*started) {
        (void)pthread_join(*thr, NULL);
        *started = false;
    }
    if (*ctx != NULL) {
        lipc_server_destroy(*ctx);
        *ctx = NULL;
    }
    if (path_for_unlink != NULL && path_for_unlink[0] != '\0') {
        (void)unlink(path_for_unlink);
    }
}

void x52d_ipc_exit(void)
{
    ipc_stop_join(&ipc_listen_thread, &ipc_listen_thread_started, &ipc_srv_ctx, ipc_path_stored);
    ipc_path_stored = NULL;
}

void x52d_ipc_push_device_state(uint16_t index, uint64_t value, const void *payload,
    size_t payload_len)
{
    lipc_server *srv = ipc_srv_ctx;

    if (srv == NULL) {
        return;
    }
    if (index > 1u) {
        return;
    }
    (void)lipc_server_broadcast_notify(srv, X52D_IPC_PUSH_DEVICE_STATE, (uint16_t)LIPC_OK, index,
        value, payload, payload_len);
}

int x52d_ipc_init(const char *sock_path)
{
    const char *path;
    int listen_fd;
    lipc_server *srv;
    int rc;

    ipc_srv_ctx = NULL;
    ipc_listen_thread_started = false;
    ipc_path_stored = NULL;

    path = x52d_ipc_sock_path(sock_path);

    listen_fd = lipc_socket_listen(path, X52D_MAX_CLIENTS, LIPC_SOCKET_NONBLOCK);
    if (listen_fd < 0) {
        PINELOG_ERROR(_("Error listening on framed IPC socket %s: %s"), path, strerror(errno));
        return -1;
    }

    srv = lipc_server_create(listen_fd, LIPC_MAX_PAYLOAD_DEFAULT,
        x52d_ipc_handlers_table(), x52d_ipc_handlers_count(), NULL);
    if (srv == NULL) {
        PINELOG_ERROR(_("Error creating framed IPC server for %s: %s"), path, strerror(errno));
        close(listen_fd);
        unlink(path);
        return -1;
    }

    ipc_path_stored = path;
    ipc_srv_ctx = srv;
    rc = pthread_create(&ipc_listen_thread, NULL, ipc_listen_thread_main, srv);
    if (rc != 0) {
        PINELOG_ERROR(_("Error %d starting framed IPC thread for %s: %s"), rc, path, strerror(rc));
        lipc_server_destroy(srv);
        ipc_srv_ctx = NULL;
        unlink(path);
        ipc_path_stored = NULL;
        return -1;
    }

    ipc_listen_thread_started = true;
    PINELOG_INFO(_("Framed IPC server listening on %s"), path);
    return 0;
}
