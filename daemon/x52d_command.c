/*
 * Saitek X52 Pro MFD & LED driver - Command processor
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "pinelog.h"
#include "x52d_const.h"
#include "x52d_command.h"

#define MAX_CONN    (X52D_MAX_CLIENTS + 1)

#define INVALID_CLIENT -1

static int client_fd[X52D_MAX_CLIENTS];
static int active_clients;

void x52d_command_init(void)
{
    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        client_fd[i] = INVALID_CLIENT;
    }

    active_clients = 0;
}

static void register_client(int sock_fd)
{
    int fd;

    if (active_clients >= X52D_MAX_CLIENTS) {
        /* Ignore the incoming connection */
        return;
    }

    fd = accept(sock_fd, NULL, NULL);
    if (fd < 0) {
        PINELOG_ERROR(_("Error accepting client connection on command socket: %s"),
                      strerror(errno));
        return;
    }

    PINELOG_TRACE("Accepted client %d on command socket", fd);

    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        if (client_fd[i] == INVALID_CLIENT) {
            client_fd[i] = fd;
            active_clients++;
            break;
        }
    }
}

static void deregister_client(int fd)
{
    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        if (client_fd[i] == fd) {
            client_fd[i] = INVALID_CLIENT;
            active_clients--;
            close(fd);

            PINELOG_TRACE("Disconnected client %d from command socket", fd);
            break;
        }
    }

}

static void client_error(int fd)
{
    int error;
    socklen_t errlen = sizeof(error);

    getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
    PINELOG_ERROR(_("Error when polling command socket: FD %d, error %d, len %lu"),
                  fd, error, (unsigned long int)errlen);
    deregister_client(fd);
}

static int poll_clients(int sock_fd, struct pollfd *pfd)
{
    int pfd_count;
    int rc;

    memset(pfd, 0, sizeof(*pfd) * MAX_CONN);

    pfd_count = 1;
    pfd[0].fd = sock_fd;
    pfd[0].events = POLLIN | POLLERR;
    for (int i = 0; i < X52D_MAX_CLIENTS; i++) {
        if (client_fd[i] != INVALID_CLIENT) {
            pfd[pfd_count].fd = client_fd[i];
            pfd[pfd_count].events = POLLIN | POLLERR | POLLHUP;
            pfd_count++;
        }
    }

    PINELOG_TRACE("Polling %d file descriptors", pfd_count);
    rc = poll(pfd, pfd_count, -1);
    if (rc < 0) {
        if (errno != EINTR) {
            PINELOG_ERROR(_("Error when polling for command: %s"), strerror(errno));
            return -1;
        }
    } else if (rc == 0) {
        PINELOG_INFO(_("Timed out when polling for command"));
    }

    return rc;
}

int x52d_command_loop(int sock_fd)
{
    struct pollfd pfd[MAX_CONN];
    int rc;
    int i;

    rc = poll_clients(sock_fd, pfd);
    if (rc <= 0) {
        return -1;
    }

    for (i = 0; i < MAX_CONN; i++) {
        if (pfd[i].revents & POLLHUP) {
            /* Remote hungup */
            deregister_client(pfd[i].fd);
        } else if (pfd[i].revents & POLLERR) {
            /* Error reading from the socket */
            client_error(pfd[i].fd);
        } else if (pfd[i].revents & POLLIN) {
            if (pfd[i].fd == sock_fd) {
                register_client(sock_fd);
            } else {
                char buffer[1024];
                int sent;

                rc = recv(pfd[i].fd, buffer, sizeof(buffer), 0);
                if (rc < 0) {
                    PINELOG_ERROR(_("Error reading from client %d: %s"),
                                  pfd[i].fd, strerror(errno));
                    continue;
                }

                // TODO: Parse and handle command.
                // Echo it back to the client for now.
                sent = send(pfd[i].fd, buffer, rc, 0);
                if (sent != rc) {
                    PINELOG_ERROR(_("Short write to client %d; expected %d bytes, wrote %d bytes"),
                                  pfd[i].fd, rc, sent);
                }
            }
        }
    }

    return 0;
}
