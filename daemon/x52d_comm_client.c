/*
 * Saitek X52 Pro MFD & LED driver - Client communication library
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "x52dcomm.h"
#include "x52dcomm-internal.h"

static int _setup_socket(struct sockaddr_un *remote, int len)
{
    int sock;
    int saved_errno;

    /* Create a socket */
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        /* Failure creating the socket, abort early */
        return -1;
    }

    /* Connect to the socket */
    if (connect(sock, (struct sockaddr *)remote, (socklen_t)len) == -1) {
        /* Failure connecting to the socket. Cleanup */
        saved_errno = errno;
        /* close may modify errno, so we save it prior to the call */
        close(sock);
        sock = -1;
        errno = saved_errno;
    }

    return sock;
}

int x52d_dial_command(const char *sock_path)
{
    int len;
    struct sockaddr_un remote;

    len = x52d_setup_command_sock(sock_path, &remote);
    if (len < 0) {
        /* Error when setting up sockaddr */
        return -1;
    }

    return _setup_socket(&remote, len);
}

int x52d_dial_notify(const char *sock_path)
{
    int len;
    struct sockaddr_un remote;

    len = x52d_setup_notify_sock(sock_path, &remote);
    if (len < 0) {
        /* Error when setting up sockaddr */
        return -1;
    }

    return _setup_socket(&remote, len);
}

int x52d_format_command(int argc, const char **argv, char *buffer, size_t buflen)
{
    int msglen;
    int i;

    if (argc == 0 || argv == NULL || buffer == NULL || buflen < X52D_BUFSZ) {
        errno = EINVAL;
        return -1;
    }

    memset(buffer, 0, buflen);
    msglen = 0;
    for (i = 0; i < argc; i++) {
        int arglen = strlen(argv[i]) + 1;
        if ((size_t)(msglen + arglen) >= buflen) {
            errno = E2BIG;
            return -1;
        }

        memcpy(&buffer[msglen], argv[i], arglen);
        msglen += arglen;
    }

    return msglen;
}

int x52d_send_command(int sock_fd, char *buffer, size_t bufin, size_t bufout)
{
    int rc;

    for (;;) {
        /*
         * Unix sockets should have sufficient capacity to send the full
         * datagram in a single message. Assume that is the case.
         */
        rc = send(sock_fd, buffer, bufin, 0);
        if (rc < 0) {
            // Error
            if (errno == EINTR) {
                // System call interrupted due to signal. Try again
                continue;
            } else {
                // Failed. Return early
                return -1;
            }
        }

        break;
    }

    /* Wait till we get a response */
    for (;;) {
        rc = recv(sock_fd, buffer, bufout, 0);
        if (rc < 0) {
            // Error
            if (errno == EINTR) {
                // System call interrupted due to signal. Try again
                continue;
            } else {
                // Failed. Return early
                return -1;
            }
        }

        break;
    }

    return rc;
}

int x52d_recv_notification(int sock_fd, x52d_notify_callback_fn callback)
{
    int rc;
    char buffer[X52D_BUFSZ];
    int argc;
    char *argv[X52D_BUFSZ];

    if (callback == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Wait till we get a response */
    for (;;) {
        rc = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (rc < 0) {
            // Error
            if (errno == EINTR) {
                // System call interrupted due to signal. Try again
                continue;
            } else {
                // Failed. Return early
                return -1;
            }
        }

        break;
    }

    /* Split into individual arguments */
    x52d_split_args(&argc, argv, buffer, rc);

    return callback(argc, argv);
}
