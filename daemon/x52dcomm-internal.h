/*
 * Saitek X52 Pro MFD & LED driver - communication library interal functions
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52DCOMM_INTERNAL_H
#define X52DCOMM_INTERNAL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define X52D_BUFSZ  1024

const char *x52d_command_sock_path(const char *sock_path);
int x52d_setup_command_sock(const char *sock_path, struct sockaddr_un *remote);
const char *x52d_notify_sock_path(const char *sock_path);
int x52d_setup_notify_sock(const char *sock_path, struct sockaddr_un *remote);
int x52d_set_socket_nonblocking(int sock_fd);
void x52d_split_args(int *argc, char **argv, char *buffer, int buflen);

#endif // !defined X52DCOMM_INTERNAL_H
