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

int x52d_dial_command(const char *sock_path)
{
    int sock;
    int len;
    struct sockaddr_un remote;
    int saved_errno;

    len = x52d_setup_command_sock(sock_path, &remote);
    if (len < 0) {
        /* Error when setting up sockaddr */
        return -1;
    }

    /* Create a socket */
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        /* Failure creating the socket, abort early */
        return -1;
    }

    /* Connect to the socket */
    if (connect(sock, (struct sockaddr *)&remote, (socklen_t)len) == -1) {
        /* Failure connecting to the socket. Cleanup */
        saved_errno = errno;
        /* close may modify errno, so we save it prior to the call */
        close(sock);
        sock = -1;
        errno = saved_errno;
    }

    return sock;
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
