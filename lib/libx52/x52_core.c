/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "libx52.h"
#include "x52_commands.h"
#include "x52_common.h"

#define VENDOR_SAITEK 0x06a3
#define X52_PROD_X52PRO 0x0762
#define X52_PROD_X52_1  0x0255
#define X52_PROD_X52_2  0x075C

/* Check if the USB device is supported by this library */
static int libx52_check_product(uint16_t idVendor, uint16_t idProduct)
{
    if (idVendor == VENDOR_SAITEK) {
        switch (idProduct) {
        case X52_PROD_X52_1:
        case X52_PROD_X52_2:
        case X52_PROD_X52PRO:
            return 1;
        }
    }

    return 0;
}

/* Check if the attached device is an X52 Pro */
static int libx52_device_is_x52pro(uint16_t idProduct)
{
    return (idProduct == X52_PROD_X52PRO);
}

int libx52_init(libx52_device **dev)
{
    int rc;
    ssize_t count;
    int i;
    libusb_device **list;
    libusb_device *device;
    libusb_device_handle *hdl;
    struct libusb_device_descriptor desc;
    libx52_device *x52_dev;

    /* Make sure that we have a valid return pointer */
    if (dev == NULL) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Allocate memory for the library's data structures */
    x52_dev = calloc(1, sizeof(libx52_device));
    if (!x52_dev) {
        return LIBX52_ERROR_OUT_OF_MEMORY;
    }

    rc = libusb_init(&(x52_dev->ctx));
    if (rc) {
        rc = LIBX52_ERROR_INIT_FAILURE;
        goto err_recovery;
    }
    libusb_set_debug(x52_dev->ctx, LIBUSB_LOG_LEVEL_WARNING);

    count = libusb_get_device_list(x52_dev->ctx, &list);
    for (i = 0; i < count; i++) {
        device = list[i];
        if (!libusb_get_device_descriptor(device, &desc)) {
            if (libx52_check_product(desc.idVendor, desc.idProduct)) {
                rc = libusb_open(device, &hdl);
                if (rc) {
                    goto err_recovery;
                }

                x52_dev->hdl = hdl;

                if (libx52_device_is_x52pro(desc.idProduct)) {
                    set_bit(&(x52_dev->flags), X52_FLAG_IS_PRO);
                }
                break;
            }
        }
    }
    libusb_free_device_list(list, 1);

    /* Make sure we actually have an X52 device detected */
    if (!x52_dev->hdl) {
        goto err_recovery;
    }

    *dev = x52_dev;
    return LIBX52_SUCCESS;

err_recovery:
    free(x52_dev);
    return rc;
}

void libx52_exit(libx52_device *dev)
{
    libusb_close(dev->hdl);
    libusb_exit(dev->ctx);
    free(dev);
}

