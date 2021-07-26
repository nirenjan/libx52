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
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "x52d_clock.h"
#include "x52d_const.h"
#include "x52d_config.h"
#include "x52d_device.h"
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
            _("Usage: %s [-f] [-v] [-q] [-l log-file] [-o override] [-c config-file]\n"),
            X52D_APP_NAME);
    exit(exit_code);
}

int main(int argc, char **argv)
{
    int verbosity = 0;
    bool quiet = false;
    bool foreground = false;
    char *log_file = NULL;
    char *conf_file = NULL;
    int opt;

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
     */
    while ((opt = getopt(argc, argv, "fvql:o:c:h")) != -1) {
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

        case 'h':
            usage(EXIT_SUCCESS);
            break;

        default:
            usage(EXIT_FAILURE);
            break;
        }
    }

    PINELOG_DEBUG("Foreground = %s", foreground ? "true" : "false");
    PINELOG_DEBUG("Quiet = %s", quiet ? "true" : "false");
    PINELOG_DEBUG("Verbosity = %d", verbosity);
    PINELOG_DEBUG("Log file = %s", log_file);
    PINELOG_DEBUG("Config file = %s", conf_file);

    set_log_file(foreground, log_file);
    x52d_config_load(conf_file);

    // Initialize signal handlers
    listen_signal(SIGINT, termination_handler);
    listen_signal(SIGTERM, termination_handler);
    listen_signal(SIGQUIT, termination_handler);
    listen_signal(SIGHUP, reload_handler);

    // Start device threads
    x52d_dev_init();
    x52d_clock_init();

    // Apply configuration
    x52d_config_apply();

    flag_quit = 0;
    while(!flag_quit) {
        // TODO: Replace with main event loop
        // Let all threads run in background forever
        sleep(600);

        /* Check if we need to reload configuration */
        if (flag_reload) {
            PINELOG_INFO(_("Reloading X52 configuration"));
            x52d_config_load(conf_file);
            x52d_config_apply();
            flag_reload = false;
        }
    }

    // Stop device threads
    x52d_clock_exit();
    x52d_dev_exit();
    PINELOG_INFO(_("Shutting down X52 daemon"));

    return 0;
}
