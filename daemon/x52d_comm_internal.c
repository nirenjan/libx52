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

#include "x52dcomm-internal.h"
#include "x52d_const.h"

const char * x52d_command_sock_path(const char *sock_path)
{
    if (sock_path == NULL) {
        sock_path = X52D_SOCK_COMMAND;
    }

    return sock_path;
}

int x52d_setup_command_sock(const char *sock_path, struct sockaddr_un *remote)
{
    int len;
    if (remote == NULL) {
        errno = EINVAL;
        return -1;
    }

    sock_path = x52d_command_sock_path(sock_path);

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
