/*
 * LibUSB stub - device list
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "libusbx52.h"

static pthread_mutex_t vector_mutex = PTHREAD_MUTEX_INITIALIZER;

libusb_device * vector_push(libusb_context *ctx, int vid, int pid)
{
    int i;
    int num_devices;
    libusb_device *devlist;

    pthread_mutex_lock(&vector_mutex);

    // Make sure that we have an empty slot, if not, we need to reallocate the
    // device list
    for (i = 0; i < ctx->num_devices; i++) {
        if (ctx->devices[i].context == NULL) {
            break;
        }
    }

    if (i == ctx->num_devices) {
        // No empty slots, we will need to reallocate the device list
        num_devices = ctx->num_devices + 1;
        devlist = calloc(num_devices, sizeof(*devlist));
        if (devlist == NULL) {
            pthread_mutex_unlock(&vector_mutex);
            return NULL;
        }
        memcpy(devlist, ctx->devices, ctx->num_devices * sizeof(*devlist));
        ctx->num_devices = num_devices;

        free(ctx->devices);
        ctx->devices = devlist;
    }

    ctx->devices[i].context = ctx;
    ctx->devices[i].index = i;
    ctx->devices[i].desc.idVendor = vid;
    ctx->devices[i].desc.idProduct = pid;

    pthread_mutex_unlock(&vector_mutex);
    return &(ctx->devices[i]);
}

libusb_device * vector_pop(libusb_context *ctx, int vid, int pid)
{
    int i;
    libusb_device *dev;

    // Search through the device list for a matching VID/PID pair
    // If found, then delete it from the list

    pthread_mutex_lock(&vector_mutex);

    for (i = 0; i < ctx->num_devices; i++) {
        dev = &(ctx->devices[i]);
        if (dev->desc.idVendor == vid && dev->desc.idProduct == pid) {
            break;
        }
    }

    if (i < ctx->num_devices) {
        memset(dev, 0, sizeof(*dev));
    } else {
        dev = NULL;
    }

    pthread_mutex_unlock(&vector_mutex);

    return dev;
}
