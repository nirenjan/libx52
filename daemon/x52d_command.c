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
#include <pthread.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define PINELOG_MODULE X52D_MOD_COMMAND
#include "pinelog.h"
#include "x52d_const.h"
#include "x52d_command.h"
#include "x52d_config.h"
#include "x52d_client.h"
#include "x52dcomm-internal.h"

static int client_fd[X52D_MAX_CLIENTS];

static pthread_t command_thr;
static int command_sock_fd;
static const char *command_sock;

#if defined __has_attribute
#   if __has_attribute(format)
        __attribute((format(printf, 4, 5)))
#   endif
#endif
static void response_formatted(char *buffer, int *buflen, const char *type,
                               const char *fmt, ...)
{
    va_list ap;
    char response[X52D_BUFSZ];
    int resplen;
    int typelen;

    typelen = strlen(type) + 1;
    strcpy(response, type);
    resplen = typelen;

    if (*fmt) {
        va_start(ap, fmt);
        resplen += vsnprintf(response + typelen, sizeof(response) - typelen, fmt, ap) + 1;
        va_end(ap);
    }

    memcpy(buffer, response, resplen);
    *buflen = resplen;
}

static void response_strings(char *buffer, int *buflen, const char *type, int count, ...)
{
    va_list ap;
    char response[X52D_BUFSZ];
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

#define DATA(...) response_strings(buffer, buflen, "DATA", NUMARGS(__VA_ARGS__), ##__VA_ARGS__)

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
                x52d_config_apply_immediate(argv[2], argv[3]);
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
                DATA(argv[2], argv[3], rv);
            }
        } else {
            ERR_fmt("Unexpected arguments for 'config get' command; got %d, expected 4", argc);
        }
        return;
    }

    ERR_fmt("Unknown subcommand '%s' for 'config' command", argv[1]);
}

struct level_map {
    int level;
    const char *string;
};

static int lmap_get_level(const struct level_map *map, const char *string, int notfound)
{
    int i;

    for (i = 0; map[i].string != NULL; i++) {
        if (strcasecmp(map[i].string, string) == 0) {
            return map[i].level;
        }
    }

    return notfound;
}

static const char *lmap_get_string(const struct level_map *map, int level)
{
    int i;

    for (i = 0; map[i].string != NULL; i++) {
        if (map[i].level == level) {
            return map[i].string;
        }
    }

    return NULL;
}

static int array_find_index(const char **array, int nmemb, const char *string)
{
    int i;

    for (i = 0; i < nmemb; i++) {
        if (strcasecmp(array[i], string) == 0) {
            return i;
        }
    }

    return nmemb;
}

static void cmd_logging(char *buffer, int *buflen, int argc, char **argv)
{
    static const char *modules[X52D_MOD_MAX] = {
        [X52D_MOD_CONFIG] = "config",
        [X52D_MOD_CLOCK] = "clock",
        [X52D_MOD_DEVICE] = "device",
        [X52D_MOD_IO] = "io",
        [X52D_MOD_LED] = "led",
        [X52D_MOD_MOUSE] = "mouse",
        [X52D_MOD_KEYBOARD] = "keyboard",
        [X52D_MOD_PROFILE] = "profile",
        [X52D_MOD_CLUTCH] = "clutch",
        [X52D_MOD_COMMAND] = "command",
        [X52D_MOD_CLIENT] = "client",
        [X52D_MOD_NOTIFY] = "notify",
    };

    // This corresponds to the levels in pinelog
    static const struct level_map loglevels[] = {
        {PINELOG_LVL_NOTSET, "default"},
        {PINELOG_LVL_NONE, "none"},
        {PINELOG_LVL_FATAL, "fatal"},
        {PINELOG_LVL_ERROR, "error"},
        {PINELOG_LVL_WARNING, "warning"},
        {PINELOG_LVL_INFO, "info"},
        {PINELOG_LVL_DEBUG, "debug"},
        {PINELOG_LVL_TRACE, "trace"},
        {0, NULL},
    };

    if (argc < 2) {
        ERR("Insufficient arguments for 'logging' command");
        return;
    }

    // logging show [module]
    MATCH(1, "show") {
        if (argc == 2) {
            // Show default logging level
            DATA("global", lmap_get_string(loglevels, pinelog_get_level()));
        } else if (argc == 3) {
            int module = array_find_index(modules, X52D_MOD_MAX, argv[2]);
            if (module == X52D_MOD_MAX) {
                ERR_fmt("Invalid module '%s'", argv[2]);
            } else {
                DATA(argv[2], lmap_get_string(loglevels, pinelog_get_module_level(module)));
            }
        } else {
            ERR_fmt("Unexpected arguments for 'logging show' command; got %d, expected 2 or 3", argc);
        }

        return;
    }

    // logging set [module] <level>
    MATCH(1, "set") {
        if (argc == 3) {
            int level = lmap_get_level(loglevels, argv[2], INT_MAX);
            if (level == INT_MAX) {
                ERR_fmt("Unknown level '%s' for 'logging set' command", argv[2]);
            } else if (level == PINELOG_LVL_NOTSET) {
                ERR("'default' level is not valid without a module");
            } else {
                pinelog_set_level(level);
                OK("logging", "set", argv[2]);
            }
        } else if (argc == 4) {
            int level = lmap_get_level(loglevels, argv[3], INT_MAX);
            int module = array_find_index(modules, X52D_MOD_MAX, argv[2]);

            if (module == X52D_MOD_MAX) {
                ERR_fmt("Invalid module '%s'", argv[2]);
                return;
            }

            if (level == INT_MAX) {
                ERR_fmt("Unknown level '%s' for 'logging set' command", argv[3]);
            } else {
                pinelog_set_module_level(module, level);
                OK("logging", "set", argv[2], argv[3]);
            }
        } else {
            ERR_fmt("Unexpected arguments for 'logging set' command; got %d, expected 3 or 4", argc);
        }

        return;
    }

    ERR_fmt("Unknown subcommand '%s' for 'logging' command", argv[1]);
}

static void command_parser(char *buffer, int *buflen)
{
    int argc = 0;
    char *argv[X52D_BUFSZ] = { 0 };

    x52d_split_args(&argc, argv, buffer, *buflen);

    MATCH(0, "config") {
        cmd_config(buffer, buflen, argc, argv);
    } else MATCH(0, "logging") {
        cmd_logging(buffer, buflen, argc, argv);
    } else {
        ERR_fmt("Unknown command '%s'", argv[0]);
    }
}

static void client_handler(int fd)
{
    char buffer[X52D_BUFSZ] = { 0 };
    int sent;
    int rc;

    rc = recv(fd, buffer, sizeof(buffer), 0);
    if (rc < 0) {
        PINELOG_ERROR(_("Error reading from client %d: %s"),
                      fd, strerror(errno));
        return;
    }

    // Parse and handle command.
    command_parser(buffer, &rc);

    PINELOG_TRACE("Sending %d bytes in response '%s'", rc, buffer);
    sent = send(fd, buffer, rc, 0);
    if (sent != rc) {
        PINELOG_ERROR(_("Short write to client %d; expected %d bytes, wrote %d bytes"),
                      fd, rc, sent);
    }
}

int x52d_command_loop(int sock_fd)
{
    struct pollfd pfd[MAX_CONN];
    int rc;

    rc = x52d_client_poll(client_fd, pfd, sock_fd);
    if (rc <= 0) {
        return -1;
    }

    x52d_client_handle(client_fd, pfd, sock_fd, client_handler);

    return 0;
}

static void * x52d_command_thread(void *param)
{
    for (;;) {
        if (x52d_command_loop(command_sock_fd) < 0) {
            PINELOG_FATAL(_("Error %d during command loop: %s"),
                          errno, strerror(errno));
        }
    }

    return NULL;
}

int x52d_command_init(const char *sock_path)
{
    int sock_fd;
    int len;
    struct sockaddr_un local;

    x52d_client_init(client_fd);

    command_sock = sock_path;
    command_sock_fd = -1;

    len = x52d_setup_command_sock(command_sock, &local);
    if (len < 0) {
        return -1;
    }

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        /* Failure creating the socket. Abort early */
        PINELOG_ERROR(_("Error creating command socket: %s"), strerror(errno));
        return -1;
    }

    command_sock_fd = sock_fd;

    /* Mark the socket as non-blocking */
    if (x52d_set_socket_nonblocking(sock_fd) < 0) {
        PINELOG_ERROR(_("Error marking command socket as nonblocking: %s"),
                      strerror(errno));
        goto sock_failure;
    }

    if (x52d_listen_socket(&local, len, sock_fd) < 0) {
        PINELOG_ERROR(_("Error listening on command socket: %s"), strerror(errno));
        goto listen_failure;
    }

    PINELOG_INFO(_("Starting command processing thread"));
    pthread_create(&command_thr, NULL, x52d_command_thread, NULL);

    return 0;

listen_failure:
    unlink(local.sun_path);
sock_failure:
    if (command_sock_fd >= 0) {
        close(command_sock_fd);
        command_sock_fd = -1;
    }

    return -1;
}

void x52d_command_exit(void)
{
    PINELOG_INFO(_("Shutting down command processing thread"));
    pthread_cancel(command_thr);

    // Close the socket and remove the socket file
    if (command_sock_fd >= 0) {
        command_sock = x52d_command_sock_path(command_sock);
        PINELOG_TRACE("Closing command socket %s", command_sock);

        close(command_sock_fd);
        command_sock_fd = -1;

        unlink(command_sock);
        command_sock = NULL;
    }

}
