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

const char *x52d_command_sock_path(const char *sock_path);
int x52d_setup_command_sock(const char *sock_path, struct sockaddr_un *remote);

#endif // !defined X52DCOMM_INTERNAL_H
