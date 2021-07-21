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
#include <unistd.h>

#include "x52d_const.h"
#include "x52d_config.h"
#include "x52d_device.h"
#include "pinelog.h"

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

    printf("Foreground = %s\n", foreground ? "true" : "false");
    printf("Quiet = %s\n", quiet ? "true" : "false");
    printf("Verbosity = %d\n", verbosity);
    printf("Log file = %s\n", log_file);
    printf("Config file = %s\n", conf_file);

    set_log_file(foreground, log_file);
    x52d_config_load(conf_file);

    // Start device threads
    x52d_dev_init();

    // Apply configuration
    x52d_config_apply();

    for(;;) {
        // TODO: Replace with main event loop
        // Let all threads run in background forever
        sleep(600);
    }

    return 0;
}
