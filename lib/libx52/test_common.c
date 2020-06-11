/*
 * Saitek X52 Pro MFD & LED driver
 * Common functionality for test programs
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "x52_common.h"
#include "test_common.h"

#define MAX_DIAGS 32
#define MAX_DIAG_SZ 256
static int diag_count;
static char diagnostic[MAX_DIAGS][MAX_DIAG_SZ];

#define ADD_DIAG(fmt_str, ...) do { \
    if (diag_count < MAX_DIAGS) { \
        snprintf(diagnostic[diag_count], MAX_DIAG_SZ, fmt_str, ##__VA_ARGS__); \
        diag_count++; \
    } \
} while(0)

/* Test vendor command function */
int x52_test_vendor_command(libx52_device *dev, uint16_t index, uint16_t value)
{
    struct x52_vendor_data *vdata = (struct x52_vendor_data *)dev->hdl;
    struct ivpair data = {index, value};

    if (vdata->written < MAX_SZ) {
        vdata->data[vdata->written] = data;
        vdata->written++;
    }

    return LIBX52_SUCCESS;
}

/* Check expected data */
bool x52_test_assert_expected(libx52_device *dev, struct ivpair *data)
{
    int written = 0;
    struct x52_vendor_data *vdata = (struct x52_vendor_data *)dev->hdl;

    while(data->index != 0 && data->value != 0 && written < vdata->written) {
        if ((data->index != vdata->data[written].index) ||
            (data->value != vdata->data[written].value)) {
            ADD_DIAG("Mismatched data at position %d:", written);
            ADD_DIAG("\tExpected: {%04x, %04x}", data->index, data->value);
            ADD_DIAG("\tObserved: {%04x, %04x}", vdata->data[written].index, vdata->data[written].value);
            return false;
        }

        data++;
        written++;
    }

    if (data->index != 0 || data->value != 0) {
        ADD_DIAG("Insufficient data written, got only %d, additional expected:", written);
        while (data->index != 0 && data->value != 0) {
            ADD_DIAG("\t%04x %04x", data->index, data->value);
            data++;
        }
        return false;
    }

    if (vdata->written > written) {
        ADD_DIAG("More data written, expected only %d, got %d", written, vdata->written);
        return false;
    }

    return true;
}

void x52_test_print_diagnostics(void)
{
    int i;
    for (i = 0; i < diag_count; i++) {
        printf("# %s\n", diagnostic[i]);
    }
}

/*
 * Initialize libx52, close any device handles, create a dummy handle
 * and override the vendor command function.
 */
libx52_device *x52_test_init(void)
{
    libx52_device *dev;
    struct x52_vendor_data *vdata;
    int rc;

    rc = libx52_init(&dev);
    if (rc != LIBX52_SUCCESS) {
        fputs(libx52_strerror(rc), stderr);
        exit(1);
    }

    (void)libx52_disconnect(dev);

    /* Allocate memory for vendor data */
    vdata = calloc(1, sizeof(*vdata));
    if (vdata == NULL) {
        perror("vendor data calloc");
        libx52_exit(dev);
        return NULL;
    }

    /* Reset the diagnostics buffers */
    memset(diagnostic, 0, sizeof(diagnostic));
    diag_count = 0;

    /* We don't need the device handle in test code, repurpose it */
    dev->hdl = (libusb_device_handle *)vdata;

    /* Setup vendor command function */
    dev->vendor_cmd_fn = x52_test_vendor_command;

    return dev;
}

void x52_test_cleanup(libx52_device *dev)
{
    free(dev->hdl);
    dev->hdl = NULL;
    libx52_exit(dev);
}
