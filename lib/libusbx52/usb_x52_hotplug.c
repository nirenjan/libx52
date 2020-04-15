/*
 * LibUSB stub driver for hotplug APIs
 *
 * Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include <libusb.h>
#include "libusbx52.h"

#define CB_HANDLE_MAGIC 0xca11bacc

static void *service_hotplug_events(void *args)
{
    libusb_context *ctx = args;
    int parsed;
    int vid;
    int pid;
    char action;
    char *dev_fifo_file;
    FILE *fifo;
    libusb_device *dev;

    // Get the filename of the FIFO
    dev_fifo_file = getenv(INPUT_DEVICE_FIFO_ENV);
    if (dev_fifo_file == NULL || dev_fifo_file[0] == '\0') {
        dev_fifo_file = DEFAULT_INPUT_DEVICE_FIFO_FILE;
    }

    // Remove the FIFO if it exists
    unlink(dev_fifo_file);

    // Create the FIFO
    mkfifo(dev_fifo_file, 0777);

    fifo = fopen(dev_fifo_file, "r");
    if (fifo == NULL) {
        return NULL;
    }

    while (!ctx->stop_thread) {
        parsed = fscanf(fifo, "%c %x %x", &action, &vid, &pid);
        if (parsed == 3) {
            switch (action) {
            case '+':
                dev = vector_push(ctx, vid, pid);
                if ((ctx->events & LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) &&
                    (ctx->hotplug_vid == LIBUSB_HOTPLUG_MATCH_ANY || ctx->hotplug_vid == vid) &&
                    (ctx->hotplug_pid == LIBUSB_HOTPLUG_MATCH_ANY || ctx->hotplug_pid == pid)) {
                    (ctx->callback)(ctx, dev,
                                    LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                                    ctx->cb_user_data);
                }
                break;

            case '-':
                dev = vector_pop(ctx, vid, pid);
                if ((ctx->events & LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) &&
                    (ctx->hotplug_vid == LIBUSB_HOTPLUG_MATCH_ANY || ctx->hotplug_vid == vid) &&
                    (ctx->hotplug_pid == LIBUSB_HOTPLUG_MATCH_ANY || ctx->hotplug_pid == pid)) {
                    (ctx->callback)(ctx, dev,
                                    LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                    ctx->cb_user_data);
                }
                break;

            case 'x':
                /* Terminate the loop */
                ctx->stop_thread = 1;
                break;

            }
        }
    }

    // Close the FIFO
    fclose(fifo);
    return NULL;
}

int libusb_hotplug_register_callback(libusb_context *ctx,
                                     libusb_hotplug_event events,
                                     libusb_hotplug_flag flags,
                                     int vendor_id,
                                     int product_id,
                                     int dev_class,
                                     libusb_hotplug_callback_fn cb_fn,
                                     void *user_data,
                                     libusb_hotplug_callback_handle *cb_handle)
{
    // Save the events, flags, etc in the context.
    ctx->hotplug_vid = vendor_id;
    ctx->hotplug_pid = product_id;
    ctx->events = events;

    ctx->callback = cb_fn;
    ctx->cb_user_data = user_data;

    // Spawn the thread
    pthread_create(&(ctx->hotplug_pthread), NULL, service_hotplug_events, ctx);

    // Since the stub library only accepts a single callback function,
    // we will return a static value that will be used to check when
    // deregistering. Only if the value matches will we terminate the thread.
    *cb_handle = CB_HANDLE_MAGIC;
    return LIBUSB_SUCCESS;
}

void libusb_hotplug_deregister_callback(libusb_context *ctx,
                                       libusb_hotplug_callback_handle cb_handle)
{
    if (cb_handle == CB_HANDLE_MAGIC) {
        ctx->stop_thread = 1;
        pthread_join(ctx->hotplug_pthread, NULL);
    }
}
