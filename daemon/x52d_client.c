/*
 * Saitek X52 Pro MFD & LED driver - Client handling
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "pinelog.h"
#include "x52d_client.h"
#include "x52dcomm-internal.h"

void x52d_client_init(int client_fd[X52D_MAX_CLIENTS])
{
    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        client_fd[i] = INVALID_CLIENT;
    }
}

bool x52d_client_register(int client_fd[X52D_MAX_CLIENTS], int sock_fd)
{
    int fd;
    int i;

    fd = accept(sock_fd, NULL, NULL);
    if (fd < 0) {
        PINELOG_ERROR(_("Error accepting client connection on socket fd %d: %s"),
                      sock_fd, strerror(errno));
        return false;
    }

    if (x52d_set_socket_nonblocking(fd) < 0) {
        PINELOG_ERROR(_("Error marking client fd %d as nonblocking: %s"),
                      fd, strerror(errno));
        goto error;
    }

    for (i = 0; i < X52D_MAX_CLIENTS; i++) {
        if (client_fd[i] == INVALID_CLIENT) {
            PINELOG_TRACE("Accepted client %d on socket %d, slot %d", fd, sock_fd, i);
            client_fd[i] = fd;
            return true;
        }
    }

    /*
     * At this point, we've looped through the entirity of client_fd, but
     * have not registered an empty slot. We need to close the socket and
     * tell the caller that we haven't been able to register the client.
     */
    PINELOG_TRACE("Maximum connections reached, closing socket %d", fd);
error:
    close(fd);

    return false;
}

bool x52d_client_deregister(int client_fd[X52D_MAX_CLIENTS], int fd)
{
    bool deregistered = false;

    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        if (client_fd[i] == fd) {
            client_fd[i] = INVALID_CLIENT;
            deregistered = true;
            close(fd);

            PINELOG_TRACE("Disconnected client %d from socket", fd);
            break;
        }
    }

    return deregistered;
}

bool x52d_client_error(int client_fd[X52D_MAX_CLIENTS], int fd)
{
    int error;
    socklen_t errlen = sizeof(error);

    getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
    PINELOG_ERROR(_("Error when polling socket: FD %d, error %d, len %lu"),
                  fd, error, (unsigned long int)errlen);
    return x52d_client_deregister(client_fd, fd);
}

int x52d_client_poll(int client_fd[X52D_MAX_CLIENTS], struct pollfd pfd[MAX_CONN], int listen_fd)
{
    int pfd_count;
    int rc;

    memset(pfd, 0, sizeof(*pfd) * MAX_CONN);

    pfd_count = 1;
    pfd[0].fd = listen_fd;
    pfd[0].events = POLLIN | POLLERR;
    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        if (client_fd[i] != INVALID_CLIENT) {
            pfd[pfd_count].fd = client_fd[i];
            pfd[pfd_count].events = POLLIN | POLLERR | POLLHUP;
            pfd_count++;
        }
    }

    PINELOG_TRACE("Polling %d file descriptors", pfd_count);

retry_poll:
    rc = poll(pfd, pfd_count, -1);
    if (rc < 0) {
        if (errno == EINTR) {
            goto retry_poll;
        }
        PINELOG_ERROR(_("Error %d when polling %d descriptors: %s"),
                      errno, pfd_count, strerror(errno));
    } else if (rc == 0) {
        PINELOG_INFO(_("Timed out when polling"));
    }

    return rc;
}

void x52d_client_handle(int client_fd[X52D_MAX_CLIENTS], struct pollfd *pfd, int listen_fd, x52d_poll_handler handler)
{
    for (int i = 0; i < MAX_CONN; i++) {
        if (pfd[i].revents & POLLHUP) {
            /* Remote hungup */
            x52d_client_deregister(client_fd, pfd[i].fd);
        } else if (pfd[i].revents & POLLERR) {
            /* Error reading from the socket */
            x52d_client_error(client_fd, pfd[i].fd);
        } else if (pfd[i].revents & POLLIN) {
            if (pfd[i].fd == listen_fd) {
                x52d_client_register(client_fd, listen_fd);
            } else {
                if (handler != NULL) {
                    handler(pfd[i].fd);
                }
            }
        }
    }
}
