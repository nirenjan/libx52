/*
 * LibUSB stub driver for testing the Saitek X52/X52 Pro
 * Common functionality
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdlib.h>

FILE * fopen_env(const char *env, const char *env_default, const char *mode)
{
    // Get the filename from the environment. Use defaults if unset or empty
    const char *filename = getenv(env);
    if (filename == NULL || filename[0] == '\0') {
        filename = env_default;
    }

    return fopen(filename, mode);
}
