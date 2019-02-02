/*
 * LibUSB stub driver for testing the Saitek X52/X52 Pro
 *
 * Copyright (C) 2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libusb.h>
#include "libusbx52.h"

int libusb_init(libusb_context **ctx)
{
    int rc;
    int dev_count;
    int vid;
    int pid;
    int parsed;
    char *dev_list_file;
    FILE *dev_list;
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

    /*
     * Get the device file name from the environment,
     * use a default name if unset or empty
     */
    dev_list_file = getenv(INPUT_DEVICE_LIST_ENV);
    if (dev_list_file == NULL || dev_list_file[0] == '\0') {
        dev_list_file = DEFAULT_INPUT_DEVICE_LIST_FILE;
    }
    dev_list = fopen(dev_list_file, "r");
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
    libusb_device **dev;

    if (unref_devices) {
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
    char *output_data_file;

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

    /*
     * Get the name of the output data file from the environment,
     * use a default name if the environment variable is unset or empty
     */
    output_data_file = getenv(OUTPUT_DATA_FILE_ENV);
    if (output_data_file == NULL || output_data_file[0] == '\0') {
        output_data_file = DEFAULT_OUTPUT_DATA_FILE;
    }
    tmp_hdl->packet_data_file = fopen(output_data_file, "w");

    /* Make sure that the file opened correctly */
    if (tmp_hdl->packet_data_file == NULL) {
        /* Error condition, return with a failure */
        free(tmp_hdl);
        return LIBUSB_ERROR_IO;
    }

    LIBUSB_DUMP_LOG_FILE(tmp_hdl, LIBUSB_LOG_LEVEL_DEBUG,
        "VID: %04x PID: %04x idx: %d ref: %d\n",
        dev->desc.idVendor, dev->desc.idProduct, dev->index, dev->ref_count);

    *handle = tmp_hdl;
    return LIBUSB_SUCCESS;
}

void libusb_close(libusb_device_handle *dev_handle)
{
    /* Decrement the refcount for the underlying device */
    dev_handle->dev->ref_count -= 1;

    libusb_device *dev = dev_handle->dev;
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
    int i;

    /* Always log the control transfer */
    fprintf(dev_handle->packet_data_file,
        "%s: RqType: %02x bRequest: %02x wValue: %04x wIndex: %04x timeout: %d\n",
        __func__, request_type, bRequest, wValue, wIndex, timeout);
    if (data != NULL) {
        fprintf(dev_handle->packet_data_file, "%s: Data[%d]: ", __func__,
                wLength);
        for (i = 0; i < wLength; i++) {
            fprintf(dev_handle->packet_data_file, "%02x ", data[i]);
        }
        fprintf(dev_handle->packet_data_file, "\n");
    }

    return 0;
}
