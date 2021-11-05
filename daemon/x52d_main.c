/*
 * Saitek X52 Pro MFD & LED driver - Service daemon
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "x52d_clock.h"
#include "x52d_const.h"
#include "x52d_config.h"
#include "x52d_device.h"
#include "x52d_io.h"
#include "x52d_mouse.h"
#include "x52d_command.h"
#include "x52dcomm-internal.h"
#include "x52dcomm.h"
#include "pinelog.h"

static volatile int flag_quit;

static void termination_handler(int signum)
{
    flag_quit = signum;
}

static volatile bool flag_reload;
static void reload_handler(int signum)
{
    flag_reload = true;
}

static volatile bool flag_save_cfg;
static void save_config_handler(int signum)
{
    flag_save_cfg = true;
}

static void set_log_file(bool foreground, const char *log_file)
{
    int rc = 0;
    if (log_file != NULL) {
        rc = pinelog_set_output_file(log_file);
    } else {
        if (foreground) {
            rc = pinelog_set_output_stream(stdout);
        } else {
            rc = pinelog_set_output_file(X52D_LOG_FILE);
        }
    }

    if (rc != 0) {
        fprintf(stderr, _("Error %d setting log file: %s\n"), rc, strerror(rc));
        exit(EXIT_FAILURE);
    }
}

static void listen_signal(int signum, void (*handler)(int))
{
    struct sigaction action;
    int rc;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    rc = sigaction(signum, &action, NULL);
    if (rc < 0) {
        PINELOG_FATAL(_("Error %d installing handler for signal %d: %s"),
                      errno, signum, strerror(errno));
    }
}

#if HAVE_FUNC_ATTRIBUTE_NORETURN
__attribute__((noreturn))
#endif
static void usage(int exit_code)
{
    fprintf(stderr,
            _("Usage: %s [-f] [-v] [-q]\n"
              "\t[-l log-file] [-o override]\n"
              "\t[-c config-file] [-p pid-file]\n"
              "\t[-s command-socket-path]\n"),
            X52D_APP_NAME);
    exit(exit_code);
}

static void start_daemon(bool foreground, const char *pid_file)
{
    pid_t pid;
    FILE *pid_fd;

    if (pid_file == NULL) {
        pid_file = X52D_PID_FILE;
    }

    /* Check if there is an existing daemon process running */
    pid_fd = fopen(pid_file, "r");
    if (pid_fd != NULL) {
        int rc;

        /* File exists, read the PID and check if it exists */
        rc = fscanf(pid_fd, "%u", &pid);
        fclose(pid_fd);

        if (rc != 1) {
            perror("fscanf");
        } else {
            rc = kill(pid, 0);
            if (rc == 0 || (rc < 0 && errno == EPERM)) {
                PINELOG_FATAL(_("Daemon is already running as PID %u"), pid);
            }
        }
    }

    if (!foreground) {
        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
            /* Error occurred during first fork */
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            /* Terminate the parent process */
            exit(EXIT_SUCCESS);
        }

        /* Make child process a session leader */
        if (setsid() < 0) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }
    }

    /* Initialize signal handlers. This step is the same whether in foreground
     * or background mode
     */
    listen_signal(SIGINT, termination_handler);
    listen_signal(SIGTERM, termination_handler);
    listen_signal(SIGQUIT, termination_handler);
    listen_signal(SIGHUP, reload_handler);
    listen_signal(SIGUSR1, save_config_handler);

    if (!foreground) {
        /* Fork off for the second time */
        pid = fork();
        if (pid < 0) {
            /* Error occurred during second fork */
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            /* Terminate the parent */
            exit(EXIT_SUCCESS);
        }
    }

    /* Write the PID to the pid_file */
    pid_fd = fopen(pid_file, "w");
    if (pid_fd == NULL) {
        /* Unable to open PID file */
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if (fprintf(pid_fd, "%u\n", getpid()) < 0) {
        perror("fprintf");
        exit(EXIT_FAILURE);
    }
    if (fclose(pid_fd) != 0) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    if (!foreground) {
        /* Set new file permissions */
        umask(0);

        /* Change the working directory */
        if (chdir("/")) {
            /* Error changing the directory */
            perror("chdir");
            exit(EXIT_FAILURE);
        }

        /* Close all open file descriptors */
        for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
            close(x);
        }
    }
}

/* Bind and listen to the command socket */
static int listen_command(const char *command_sock)
{
    int sock_fd;
    int len;
    struct sockaddr_un local;
    int flags;

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

    /* Mark the socket as non-blocking */
    flags = fcntl(sock_fd, F_GETFL);
    if (flags < 0) {
        PINELOG_ERROR(_("Error getting command socket flags: %s"), strerror(errno));
        goto sock_failure;
    }
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        PINELOG_ERROR(_("Error setting command socket flags: %s"), strerror(errno));
        goto sock_failure;
    }

    /* Cleanup any existing socket */
    unlink(local.sun_path);
    if (bind(sock_fd, (struct sockaddr *)&local, (socklen_t)len) < 0) {
        /* Failure binding socket */
        PINELOG_ERROR(_("Error binding to command socket: %s"), strerror(errno));
        goto listen_failure;
    }

    if (listen(sock_fd, X52D_MAX_CLIENTS) < 0) {
        PINELOG_ERROR(_("Error listening on command socket: %s"), strerror(errno));
        goto listen_failure;
    }

    return sock_fd;

listen_failure:
    unlink(local.sun_path);
sock_failure:
    close(sock_fd);
    return -1;
}

int main(int argc, char **argv)
{
    int verbosity = 0;
    bool quiet = false;
    bool foreground = false;
    char *log_file = NULL;
    char *conf_file = NULL;
    const char *pid_file = NULL;
    const char *command_sock = NULL;
    int opt;
    int command_sock_fd;

    /* Initialize gettext */
    #if ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
    #endif

    /* Set system defaults */
    pinelog_set_level(PINELOG_LVL_WARNING);

    /*
     * Parse command line arguments
     *
     * -f   run in foreground
     * -c   path to config file
     * -o   option overrides
     * -v   verbose logging
     * -q   silent behavior
     * -l   path to log file
     * -p   path to PID file (only used if running in background)
     * -s   path to command socket
     */
    while ((opt = getopt(argc, argv, "fvql:o:c:p:s:h")) != -1) {
        switch (opt) {
        case 'f':
            foreground = true;
            break;

        case 'v':
            if (!quiet) {
                if (verbosity <= PINELOG_LVL_TRACE) {
                    verbosity++;
                    pinelog_set_level(pinelog_get_level() + 1);
                }
            }
            break;

        case 'q':
            quiet = true;
            pinelog_set_level(PINELOG_LVL_ERROR);
            break;

        case 'l':
            log_file = optarg;
            break;

        case 'o':
            if (x52d_config_save_override(optarg)) {
                fprintf(stderr,
                        _("Unable to parse configuration override '%s'\n"),
                        optarg);
                exit(EXIT_FAILURE);
            }
            break;

        case 'c':
            conf_file = optarg;
            break;

        case 'p':
            pid_file = optarg;
            break;

        case 's':
            command_sock = optarg;
            break;

        case 'h':
            usage(EXIT_SUCCESS);
            break;

        default:
            usage(EXIT_FAILURE);
            break;
        }
    }

    PINELOG_DEBUG(_("Foreground = %s"), foreground ? _("true") : _("false"));
    PINELOG_DEBUG(_("Quiet = %s"), quiet ? _("true") : _("false"));
    PINELOG_DEBUG(_("Verbosity = %d"), verbosity);
    PINELOG_DEBUG(_("Log file = %s"), log_file);
    PINELOG_DEBUG(_("Config file = %s"), conf_file);
    PINELOG_DEBUG(_("PID file = %s"), pid_file);
    PINELOG_DEBUG(_("Command socket = %s"), command_sock);

    start_daemon(foreground, pid_file);

    set_log_file(foreground, log_file);
    x52d_config_load(conf_file);

    // Start device threads
    x52d_dev_init();
    x52d_clock_init();
    x52d_command_init();
    #if defined(HAVE_EVDEV)
    x52d_io_init();
    x52d_mouse_evdev_init();
    #endif

    command_sock_fd = listen_command(command_sock);
    if (command_sock_fd < 0) {
        goto cleanup;
    }

    // Apply configuration
    x52d_config_apply();

    flag_quit = 0;
    while(!flag_quit) {
        x52d_command_loop(command_sock_fd);

        /* Check if we need to reload configuration */
        if (flag_reload) {
            PINELOG_INFO(_("Reloading X52 configuration"));
            x52d_config_load(conf_file);
            x52d_config_apply();
            flag_reload = false;
        }

        if (flag_save_cfg) {
            PINELOG_INFO(_("Saving X52 configuration to disk"));
            x52d_config_save(conf_file);
            flag_save_cfg = false;
        }
    }

    PINELOG_INFO(_("Received termination signal %s"), strsignal(flag_quit));

cleanup:
    // Stop device threads
    x52d_clock_exit();
    x52d_dev_exit();
    #if defined(HAVE_EVDEV)
    x52d_mouse_evdev_exit();
    x52d_io_exit();
    #endif

    // Close the socket and remove the socket file
    if (command_sock_fd >= 0) {
        command_sock = x52d_command_sock_path(command_sock);
        PINELOG_TRACE("Closing command socket %s", command_sock);
        close(command_sock_fd);
        unlink(command_sock);
    }

    // Remove the PID file
    PINELOG_TRACE("Removing PID file %s", pid_file);
    unlink(pid_file);

    PINELOG_INFO(_("Shutting down X52 daemon"));

    return 0;
}
