/*
 * Saitek X52 Pro MFD & LED driver - Command processor
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "pinelog.h"
#include "x52d_const.h"
#include "x52d_command.h"
#include "x52d_config.h"

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

#if defined __has_attribute
#   if __has_attribute(format)
        __attribute((format(printf, 4, 5)))
#   endif
#endif
static void response_formatted(char *buffer, int *buflen, const char *type,
                               const char *fmt, ...)
{
    va_list ap;
    char response[1024];
    int resplen;
    int typelen;

    typelen = strlen(type) + 1;
    strcpy(response, type);
    resplen = typelen;

    if (*fmt) {
        va_start(ap, fmt);
        resplen += vsnprintf(response + typelen, sizeof(response) - typelen, fmt, ap);
        va_end(ap);
    }

    memcpy(buffer, response, resplen);
    *buflen = resplen;
}

static void response_strings(char *buffer, int *buflen, const char *type, int count, ...)
{
    va_list ap;
    char response[1024];
    int resplen;
    int arglen;
    int i;
    char *arg;

    arglen = strlen(type) + 1;
    strcpy(response, type);
    resplen = arglen;

    va_start(ap, count);
    for (i = 0; i < count; i++) {
        arg = va_arg(ap, char *);
        arglen = strlen(arg) + 1;
        if ((size_t)(arglen + resplen) >= sizeof(response)) {
            PINELOG_ERROR("Too many arguments for response_strings %s", type);
            break;
        }

        strcpy(response + resplen, arg);
        resplen += arglen;
    }
    va_end(ap);

    memcpy(buffer, response, resplen);
    *buflen = resplen;
}

#define NUMARGS(...) (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *))
#define ERR(...) response_strings(buffer, buflen, "ERR", NUMARGS(__VA_ARGS__), ##__VA_ARGS__)
#define ERR_fmt(fmt, ...) response_formatted(buffer, buflen, "ERR", fmt, ##__VA_ARGS__)

#define OK(...) response_strings(buffer, buflen, "OK", NUMARGS(__VA_ARGS__), ##__VA_ARGS__)
#define OK_fmt(fmt, ...) response_formatted(buffer, buflen, "OK", fmt, ##__VA_ARGS__)

#define MATCH(idx, cmd) if (strcasecmp(argv[idx], cmd) == 0)

static bool check_file(const char *file_path, int mode)
{
    if (*file_path == '\0') {
        return false;
    }

    if (mode && access(file_path, mode)) {
        return false;
    }

    return true;
}

static void cmd_config(char *buffer, int *buflen, int argc, char **argv)
{
    if (argc < 2) {
        ERR("Insufficient arguments for 'config' command");
        return;
    }

    MATCH(1, "load") {
        if (argc == 3) {
            if (!check_file(argv[2], R_OK)) {
                ERR_fmt("Invalid file '%s' for 'config load' command", argv[2]);
                return;
            }

            x52d_config_load(argv[2]);
            x52d_config_apply();
            OK("config", "load", argv[2]);
        } else {
            // Invalid number of args
            ERR_fmt("Unexpected arguments for 'config load' command; got %d, expected 3", argc);
        }
        return;
    }

    MATCH(1, "reload") {
        if (argc == 2) {
            raise(SIGHUP);
            OK("config", "reload");
        } else {
            ERR_fmt("Unexpected arguments for 'config reload' command; got %d, expected 2", argc);
        }
        return;
    }

    MATCH(1, "dump") {
        if (argc == 3) {
            if (!check_file(argv[2], 0)) {
                ERR_fmt("Invalid file '%s' for 'config dump' command", argv[2]);
                return;
            }

            x52d_config_save(argv[2]);
            OK("config", "dump", argv[2]);
        } else {
            ERR_fmt("Unexpected arguments for 'config dump' command; got %d, expected 3", argc);
        }

        return;
    }

    MATCH(1, "save") {
        if (argc == 2) {
            raise(SIGUSR1);
            OK("config", "save");
        } else {
            ERR_fmt("Unexpected arguments for 'config save' command; got %d, expected 2", argc);
        }
        return;
    }

    MATCH(1, "set") {
        if (argc == 5) {
            int rc = x52d_config_set(argv[2], argv[3], argv[4]);
            if (rc != 0) {
                ERR_fmt("Error %d setting '%s.%s'='%s': %s", rc,
                        argv[2], argv[3], argv[4], strerror(rc));
            } else {
                OK("config", "set", argv[2], argv[3], argv[4]);
            }
        } else {
            ERR_fmt("Unexpected arguments for 'config set' command; got %d, expected 5", argc);
        }
        return;
    }

    MATCH(1, "get") {
        if (argc == 4) {
            const char *rv = x52d_config_get(argv[2], argv[3]);
            if (rv == NULL) {
                ERR_fmt("Error getting '%s.%s'", argv[2], argv[3]);
            } else {
                response_strings(buffer, buflen, "DATA", 3, argv[2], argv[3], rv);
            }
        } else {
            ERR_fmt("Unexpected arguments for 'config get' command; got %d, expected 4", argc);
        }
        return;
    }

    MATCH(1, "apply") {
        if (argc == 2) {
            x52d_config_apply();
            OK("config", "apply");
        } else {
            ERR_fmt("Unexpected arguments for 'config apply' command; got %d, expected 2", argc);
        }
        return;
    }

    ERR_fmt("Unknown subcommand '%s' for 'config' command", argv[1]);
}

static void command_parser(char *buffer, int *buflen)
{
    int argc = 0;
    char *argv[1024] = { 0 };
    int i = 0;

    while (i < *buflen) {
        if (buffer[i]) {
            argv[argc] = buffer + i;
            argc++;
            for (; i < *buflen && buffer[i]; i++);
            // At this point, buffer[i] = '\0'
            // Skip to the next character.
            i++;
        } else {
            // We should never reach here, unless we have two NULs in a row
            argv[argc] = buffer + i;
            argc++;
            i++;
        }
    }

    MATCH(0, "config") {
        cmd_config(buffer, buflen, argc, argv);
    } else {
        ERR_fmt("Unknown command '%s'", argv[0]);
    }
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
                char buffer[1024] = { 0 };
                int sent;

                rc = recv(pfd[i].fd, buffer, sizeof(buffer), 0);
                if (rc < 0) {
                    PINELOG_ERROR(_("Error reading from client %d: %s"),
                                  pfd[i].fd, strerror(errno));
                    continue;
                }

                // Parse and handle command.
                command_parser(buffer, &rc);

                PINELOG_TRACE("Sending %d bytes in response '%s'", rc, buffer);
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
