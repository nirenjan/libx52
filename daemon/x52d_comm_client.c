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
#include "x52d_const.h"

int x52d_dial_command(const char *sock_path)
{
    int sock;
    socklen_t len;
    struct sockaddr_un remote;
    int saved_errno;

    if (sock_path == NULL) {
        sock_path = X52D_SOCK_COMMAND;
    }

    len = strlen(sock_path);
    if (len >= sizeof(remote.sun_path)) {
        /* Socket path will not fit inside sun_path */
        errno = E2BIG;
        return -1;
    }

    /* Create a socket */
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        /* Failure creating the socket, abort early */
        return -1;
    }

    /* Setup the sockaddr structure */
    memset(&remote, 0, sizeof(remote));
    remote.sun_family = AF_UNIX;
    /* We've already verified that sock_path will fit, so we don't need strncpy */
    strcpy(remote.sun_path, sock_path);
    len += sizeof(remote.sun_family);

    /* Connect to the socket */
    if (connect(sock, (struct sockaddr *)&remote, len) == -1) {
        /* Failure connecting to the socket. Cleanup */
        saved_errno = errno;
        /* close may modify errno, so we save it prior to the call */
        close(sock);
        sock = -1;
        errno = saved_errno;
    }

    return sock;
}

int x52d_send_command(int sock_fd, char *buffer, size_t buflen)
{
    int rc;

    for (;;) {
        /*
         * Unix sockets should have sufficient capacity to send the full
         * datagram in a single message. Assume that is the case.
         */
        rc = send(sock_fd, buffer, buflen, 0);
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
        rc = recv(sock_fd, buffer, buflen, 0);
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
