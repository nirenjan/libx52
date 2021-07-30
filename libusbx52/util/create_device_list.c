/*
 * LibUSB test utility library
 *
 * This program generates a list of USB devices for use by libusbx52.so
 *
 * Copyright (C) Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "libusbx52.h"

int main(int argc, char *argv[])
{
    FILE *data;
    unsigned int vid;
    unsigned int pid;
    int i;

    data = fopen_env(INPUT_DEVICE_LIST_ENV, DEFAULT_INPUT_DEVICE_LIST_FILE, "w");
    if (data == NULL) {
        fprintf(stderr, "Unable to open device list file for writing\n");
        fprintf(stderr, "%s\n", strerror(errno));
    }

    /* Process arguments until there are fewer than 2 remaining */
    for (i = 1; i < argc && (argc - i) >= 2; i += 2) {
        int parsed;
        parsed = sscanf(argv[i], "%x", &vid);
        if (parsed != 1) break;

        parsed = sscanf(argv[i+1], "%x", &pid);
        if (parsed != 1) break;

        fprintf(data, "%04x %04x\n", vid, pid);
    }

    fclose(data);
    return 0;
}
