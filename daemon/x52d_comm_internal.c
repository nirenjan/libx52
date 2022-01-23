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
#include <fcntl.h>
#include <unistd.h>

#include "x52dcomm-internal.h"
#include "x52d_const.h"

const char * x52d_command_sock_path(const char *sock_path)
{
    if (sock_path == NULL) {
        sock_path = X52D_SOCK_COMMAND;
    }

    return sock_path;
}

const char * x52d_notify_sock_path(const char *sock_path)
{
    if (sock_path == NULL) {
        sock_path = X52D_SOCK_NOTIFY;
    }

    return sock_path;
}

static int _setup_sockaddr(struct sockaddr_un *remote, const char *sock_path)
{
    int len;
    if (remote == NULL) {
        errno = EINVAL;
        return -1;
    }

    len = strlen(sock_path);
    if ((size_t)len >= sizeof(remote->sun_path)) {
        /* Socket path will not fit inside sun_path */
        errno = E2BIG;
        return -1;
    }

    /* Setup the sockaddr structure */
    memset(remote, 0, sizeof(*remote));
    remote->sun_family = AF_UNIX;
    /* We've already verified that sock_path will fit, so we don't need strncpy */
    strcpy(remote->sun_path, sock_path);
    len += sizeof(remote->sun_family);

    return len;
}

int x52d_setup_command_sock(const char *sock_path, struct sockaddr_un *remote)
{
    return _setup_sockaddr(remote, x52d_command_sock_path(sock_path));
}

int x52d_setup_notify_sock(const char *sock_path, struct sockaddr_un *remote)
{
    return _setup_sockaddr(remote, x52d_notify_sock_path(sock_path));
}

int x52d_set_socket_nonblocking(int sock_fd)
{
    int flags;

    /* Mark the socket as non-blocking */
    flags = fcntl(sock_fd, F_GETFL);
    if (flags < 0) {
        goto sock_failure;
    }
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        goto sock_failure;
    }

    return 0;

sock_failure:
    close(sock_fd);
    return -1;
}

void x52d_split_args(int *argc, char **argv, char *buffer, int buflen)
{
    int i = 0;

    while (i < buflen) {
        if (buffer[i]) {
            argv[*argc] = buffer + i;
            (*argc)++;
            for (; i < buflen && buffer[i]; i++);
            // At this point, buffer[i] = '\0'
            // Skip to the next character.
            i++;
        } else {
            // We should never reach here, unless we have two NULs in a row
            argv[*argc] = buffer + i;
            (*argc)++;
            i++;
        }
    }
}
