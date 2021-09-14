/*
 * LibUSB stub driver for testing the Saitek X52/X52 Pro
 *
 * Copyright (C) 2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#define _GNU_SOURCE
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libusb.h>
#include "libusbx52.h"

int libusb_init(libusb_context **ctx)
{
    int rc;
    int dev_count;
    unsigned int vid;
    unsigned int pid;
    int parsed;
    FILE *dev_list = NULL;
    int i;

    /*
     * Technically, libusb_init can be called with a NULL context pointer,
     * in which case, libusb will allocate a default context. However, in
     * the case of libx52, the libusb APIs are always called with a non-NULL
     * context pointer, which is then initialized with the allocated context
     * pointer.
     */
    libusb_context *tmp_ctx = calloc(1, sizeof(*tmp_ctx));
    if (tmp_ctx == NULL) {
        rc = LIBUSB_ERROR_NO_MEM;
        goto init_err_recovery;
    }

    dev_list = fopen_env(INPUT_DEVICE_LIST_ENV, DEFAULT_INPUT_DEVICE_LIST_FILE, "r");
    if (dev_list == NULL) {
        rc = LIBUSB_ERROR_IO;
        goto init_err_recovery;
    }

    /* Determine the number of devices in the file */
    dev_count = 0;
    do {
        parsed = fscanf(dev_list, "%x %x", &vid, &pid);
        /*
         * If we have read fewer than 2 items, then quit.
         * We've read all we can
         */
        if (parsed < 2) {
            break;
        }
        dev_count++;
    } while (!feof(dev_list));

    /* Make sure we have at least 1 device */
    if (dev_count == 0) {
        rc = LIBUSB_ERROR_NOT_FOUND;
        goto init_err_recovery;
    }

    /* We now have the number of devices, allocate memory for them */
    tmp_ctx->devices = calloc(dev_count, sizeof(*(tmp_ctx->devices)));
    if (tmp_ctx->devices == NULL) {
        rc = LIBUSB_ERROR_NO_MEM;
        goto init_err_recovery;
    }
    tmp_ctx->num_devices = dev_count;

    /* Rewind and read the file again, but now put them into the device list */
    rewind(dev_list);

    for (i = 0; i < dev_count && !feof(dev_list); i++) {
        /* Set the base fields */
        tmp_ctx->devices[i].context = tmp_ctx;
        tmp_ctx->devices[i].index = i;

        parsed = fscanf(dev_list, "%x %x", &vid, &pid);
        if (parsed < 2) {
            /* Parse error, skip this device */
            continue;
        }
        /* Set the VID & PID */
        tmp_ctx->devices[i].desc.idVendor = vid;
        tmp_ctx->devices[i].desc.idProduct = pid;
    }

    /* Done, close the file and return */
    fclose(dev_list);

    *ctx = tmp_ctx;
    return LIBUSB_SUCCESS;

init_err_recovery:
    /* Close the device list file if it is open */
    if (dev_list) {
        fclose(dev_list);
    }

    /* Free up any allocated memory */
    libusb_exit(tmp_ctx);

    return rc;
}

void libusb_exit(libusb_context *ctx)
{
    if (ctx) {
        if (ctx->devices) {
            free(ctx->devices);
        }
        free(ctx);
    }
}

void libusb_set_debug(libusb_context *ctx, int level)
{
    /* Set the debug level appropriately */
    ctx->debug_level = level;
}

#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106)
int libusb_set_option(libusb_context *ctx, enum libusb_option option, ...)
{
    va_list args;
    va_start(args, option);

    /* Check if the option was provided */
    if (option == LIBUSB_OPTION_LOG_LEVEL) {
        int level = va_arg(args, int);

        /* Set the debug level */
        ctx->debug_level = level;
    }

    va_end(args);

    return 0;
}
#endif

ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
    /* Allocate a list of num_devices, each pointing to a corresponding
     * libusb_device structure.
     *
     * Actually, the allocation adds one more than needed, since the list
     * is supposed to be NULL terminated, according to the libusb docs.
     */
    libusb_device **tmp_list = calloc(ctx->num_devices + 1, sizeof(*tmp_list));
    libusb_device *dev;
    int i;

    if (tmp_list == NULL) {
        return LIBUSB_ERROR_NO_MEM;
    }

    /* Initialize the list with pointers to the individual devices */
    for (i = 0; i < ctx->num_devices; i++) {
        dev = &(ctx->devices[i]);
        /* Increment the refcount */
        dev->ref_count += 1;
        tmp_list[i] = dev;
    }

    *list = tmp_list;
    return ctx->num_devices;
}

void libusb_free_device_list(libusb_device **list, int unref_devices)
{
    if (unref_devices) {
        libusb_device **dev;
        for (dev = list; *dev; dev++) {
            /* Decrement the refcount */
            (*dev)->ref_count -= 1;
        }
    }

    free(list);
}

int libusb_get_device_descriptor(libusb_device *dev,
                                 struct libusb_device_descriptor *desc)
{
    /* Copy the descriptor to the destination address */
    *desc = dev->desc;
    return 0;
}

#define LIBUSB_DUMP_LOG_FILE(hdl, loglevel, fmt_str, ...) do { \
    if (hdl->ctx->debug_level != LIBUSB_LOG_LEVEL_NONE && \
        hdl->ctx->debug_level >= loglevel) { \
        fprintf(hdl->packet_data_file, "%s: " fmt_str, __func__, __VA_ARGS__); \
    } \
} while (0)

int libusb_open(libusb_device *dev, libusb_device_handle **handle)
{
    /* Allocate a handle for the application */
    libusb_device_handle *tmp_hdl = calloc(1, sizeof(*tmp_hdl));
    if (tmp_hdl == NULL) {
        return LIBUSB_ERROR_NO_MEM;
    }

    /* Increment the reference count for the underlying device */
    dev->ref_count += 1;

    /* Populate the handle structure with the right values */
    tmp_hdl->ctx = dev->context;
    tmp_hdl->dev = dev;

    tmp_hdl->packet_data_file = fopen_env(OUTPUT_DATA_FILE_ENV,
                                          DEFAULT_OUTPUT_DATA_FILE, "w");

    /* Make sure that the file opened correctly */
    if (tmp_hdl->packet_data_file == NULL) {
        /* Error condition, return with a failure */
        free(tmp_hdl);
        return LIBUSB_ERROR_IO;
    }
    /* Set the packet data file to be line buffered */
    setlinebuf(tmp_hdl->packet_data_file);

    LIBUSB_DUMP_LOG_FILE(tmp_hdl, LIBUSB_LOG_LEVEL_DEBUG,
        "VID: %04x PID: %04x idx: %d ref: %d\n",
        dev->desc.idVendor, dev->desc.idProduct, dev->index, dev->ref_count);

    *handle = tmp_hdl;
    return LIBUSB_SUCCESS;
}

void libusb_close(libusb_device_handle *dev_handle)
{
    libusb_device *dev;
    /* Decrement the refcount for the underlying device */
    dev_handle->dev->ref_count -= 1;

    dev = dev_handle->dev;
    LIBUSB_DUMP_LOG_FILE(dev_handle, LIBUSB_LOG_LEVEL_DEBUG,
        "VID: %04x PID: %04x idx: %d ref: %d\n",
        dev->desc.idVendor, dev->desc.idProduct, dev->index, dev->ref_count);

    /* Close the file */
    fclose(dev_handle->packet_data_file);

    /* Free any memory used */
    free(dev_handle);
}

int libusb_control_transfer(libusb_device_handle *dev_handle,
                            uint8_t request_type,
                            uint8_t bRequest,
                            uint16_t wValue,
                            uint16_t wIndex,
                            unsigned char *data,
                            uint16_t wLength,
                            unsigned int timeout)
{
    /* Always log the control transfer */
    fprintf(dev_handle->packet_data_file,
        "%s: RqType: %02x bRequest: %02x wValue: %04x wIndex: %04x timeout: %u\n",
        __func__, request_type, bRequest, wValue, wIndex, timeout);
    if (data != NULL) {
        int i;
        fprintf(dev_handle->packet_data_file, "%s: Data[%u]: ", __func__,
                wLength);
        for (i = 0; i < wLength; i++) {
            fprintf(dev_handle->packet_data_file, "%02x ", data[i]);
        }
        fprintf(dev_handle->packet_data_file, "\n");
    }

    return 0;
}

int libusb_kernel_driver_active(libusb_device_handle *hdl, int interface_number)
{
    return (hdl->dev->ref_count > 0);
}

/* Indicate that the stub library can support hotplug, even though it doesn't */
int libusb_has_capability(uint32_t capability)
{
    return capability == LIBUSB_CAP_HAS_HOTPLUG;
}

/*
 * libusb 1.0.24 (and newer) changed the signature of
 * libusb_hotplug_register_callback. Older versions used the enum, but this
 * was changed to int to address libusb/libusb#714.
 *
 * This causes issues when building on older systems, since it complains
 * about conflicting types. In order to resolve this, we need to dynamically
 * determine the parameters based on the libusb version.
 */
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000108)
#define LIBUSB_HOTPLUG_EVENT int
#define LIBUSB_HOTPLUG_FLAG int
#else
#define LIBUSB_HOTPLUG_EVENT libusb_hotplug_event
#define LIBUSB_HOTPLUG_FLAG libusb_hotplug_flag
#endif

/* Dummy function to simulate registering callbacks */
int libusb_hotplug_register_callback(libusb_context *ctx,
                                     LIBUSB_HOTPLUG_EVENT events,
                                     LIBUSB_HOTPLUG_FLAG flags,
                                     int vendor_id, int product_id, int dev_class,
                                     libusb_hotplug_callback_fn cb_fn, void *user_data,
                                     libusb_hotplug_callback_handle *callback_handle)
{
    return LIBUSB_SUCCESS;
}
