/*
 * Saitek X52 Pro MFD & LED driver - Client handling
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_CLIENT_H
#define X52D_CLIENT_H

#include <stdbool.h>
#include <poll.h>

#include "x52d_const.h"

#define MAX_CONN    (X52D_MAX_CLIENTS + 1)

#define INVALID_CLIENT  -1

typedef void (*x52d_poll_handler)(int);

void x52d_client_init(int client_fd[X52D_MAX_CLIENTS]);
bool x52d_client_register(int client_fd[X52D_MAX_CLIENTS], int sock_fd);
bool x52d_client_deregister(int client_fd[X52D_MAX_CLIENTS], int fd);
bool x52d_client_error(int client_fd[X52D_MAX_CLIENTS], int fd);
int x52d_client_poll(int client_fd[X52D_MAX_CLIENTS], struct pollfd pfd[MAX_CONN], int listen_fd);
void x52d_client_handle(int client_fd[X52D_MAX_CLIENTS], struct pollfd *pfd, int listen_fd, x52d_poll_handler handler);

#endif //!defined X52D_CLIENT_H
