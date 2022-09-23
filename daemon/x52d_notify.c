/*
 * Saitek X52 Pro MFD & LED driver - Notification manager
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define PINELOG_MODULE X52D_MOD_NOTIFY
#include "pinelog.h"
#include "x52d_const.h"
#include "x52d_notify.h"
#include "x52d_client.h"
#include "x52dcomm.h"
#include "x52dcomm-internal.h"

static pthread_t notify_thr;
static pthread_t notify_listen;
static pthread_mutex_t notify_mutex = PTHREAD_MUTEX_INITIALIZER;

static int notify_pipe[2];
static int notify_sock;

static int client_fd[X52D_MAX_CLIENTS];

/* Bind and listen to the notify socket */
static int listen_notify(const char *notify_sock_path)
{
    int sock_fd;
    int len;
    struct sockaddr_un local;

    len = x52d_setup_notify_sock(notify_sock_path, &local);
    if (len < 0) {
        return -1;
    }

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        /* Failure creating the socket. Abort early */
        PINELOG_ERROR(_("Error creating notification socket: %s"), strerror(errno));
        return -1;
    }

    if (x52d_set_socket_nonblocking(sock_fd) < 0) {
        PINELOG_ERROR(_("Error marking notification socket as nonblocking: %s"), strerror(errno));
        return -1;
    }

    /* Cleanup any existing socket */
    unlink(local.sun_path);
    if (bind(sock_fd, (struct sockaddr *)&local, (socklen_t)len) < 0) {
        /* Failure binding socket */
        PINELOG_ERROR(_("Error binding to notification socket: %s"), strerror(errno));
        goto listen_failure;
    }

    if (listen(sock_fd, X52D_MAX_CLIENTS) < 0) {
        PINELOG_ERROR(_("Error listening on notification socket: %s"), strerror(errno));
        goto listen_failure;
    }

    return sock_fd;

listen_failure:
    unlink(local.sun_path);
    close(sock_fd);
    PINELOG_FATAL(_("Error setting up notification socket"));
    return -1;
}

static void * x52_notify_thr(void * param)
{
    char buffer[X52D_BUFSZ];
    uint16_t bufsiz;
    int rc;

    for (;;) {
read_pipe_size:
        rc = read(notify_pipe[0], &bufsiz, sizeof(bufsiz));
        if (rc < 0) {
            if (errno == EINTR) {
                goto read_pipe_size;
            } else {
                PINELOG_ERROR(_("Error %d reading from pipe: %s"),
                              errno, strerror(errno));
            }
        }

        if (rc < 0) {
            // Error condition, try again
            continue;
        }

read_pipe_data:
        rc = read(notify_pipe[0], buffer, bufsiz);
        if (rc < 0) {
            if (errno == EINTR) {
                goto read_pipe_data;
            } else {
                PINELOG_ERROR(_("Error %d reading from pipe: %s"),
                              errno, strerror(errno));
            }
        }

        if (rc < 0) {
            continue;
        }

        for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
            // Broadcast to every connected client
            if (client_fd[i] != INVALID_CLIENT) {
write_client_notification:
                rc = write(client_fd[i], buffer, bufsiz);
                if (rc < 0 && errno == EINTR) {
                    goto write_client_notification;
                }
            }
        }
    }

    return NULL;
}

void x52d_notify_send(int argc, const char **argv)
{
    char buffer[X52D_BUFSZ + sizeof(uint16_t)];
    uint16_t bufsiz;
    uint16_t written;
    int rc;

    bufsiz = (uint16_t)x52d_format_command(argc, argv, buffer + sizeof(uint16_t), X52D_BUFSZ);
    memcpy(buffer, &bufsiz, sizeof(bufsiz));

    pthread_mutex_lock(&notify_mutex);
    written = 0;
    while (written < bufsiz) {
        rc = write(notify_pipe[1], buffer + written, bufsiz - written);
        if (rc < 0) {
            if (errno == EINTR) {
                continue;
            }
            PINELOG_ERROR(_("Error %d writing notification pipe: %s"),
                          errno, strerror(errno));
        } else {
            written += rc;
        }
    }
    pthread_mutex_unlock(&notify_mutex);
}

static void client_handler(int fd)
{
    char buffer[X52D_BUFSZ] = { 0 };
    int rc;

    rc = recv(fd, buffer, sizeof(buffer), 0);
    PINELOG_TRACE("Received and discarded %d bytes from notification client %d", rc, fd);
}

static void * x52_notify_loop(void * param)
{
    struct pollfd pfd[MAX_CONN];
    int rc;

    for (;;) {
        rc = x52d_client_poll(client_fd, pfd, notify_sock);
        if (rc <= 0) {
            continue;
        }

        x52d_client_handle(client_fd, pfd, notify_sock, client_handler);
    }

    return NULL;
}

void x52d_notify_init(const char *notify_sock_path)
{
    int rc;

    PINELOG_TRACE("Initializing notification manager");
    x52d_client_init(client_fd);

    PINELOG_TRACE("Creating notifications pipe");
    rc = pipe(notify_pipe);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d creating notification pipe: %s"),
                      errno, strerror(errno));
    }

    PINELOG_TRACE("Opening notification listener socket");
    notify_sock = listen_notify(notify_sock_path);

    rc = pthread_create(&notify_thr, NULL, x52_notify_thr, NULL);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d initializing notify thread: %s"),
                      rc, strerror(rc));
    }

    rc = pthread_create(&notify_listen, NULL, x52_notify_loop, NULL);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d initializing notify listener: %s"),
                      rc, strerror(rc));
    }
}

void x52d_notify_exit(void)
{
    close(notify_pipe[0]);
    close(notify_pipe[1]);
    close(notify_sock);

    pthread_cancel(notify_thr);
    pthread_cancel(notify_listen);
}
