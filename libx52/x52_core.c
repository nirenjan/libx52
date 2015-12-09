/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
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

/* Check if the USB device is supported by this library */
static int libx52_check_product(uint16_t idVendor, uint16_t idProduct)
{
    if (idVendor == VENDOR_SAITEK) {
        switch (idProduct) {
        case X52_PROD_X52PRO:
            return 1;
        }
    }

    return 0;
}

libx52_device* libx52_init(void)
{
    int rc;
    ssize_t count;
    int i;
    libusb_device **list;
    libusb_device *device;
    libusb_device_handle *hdl;
    struct libusb_device_descriptor desc;
    libx52_device *x52_dev;

    /* Allocate memory for the library's data structures */
    x52_dev = calloc(1, sizeof(libx52_device));
    if (!x52_dev) {
        errno = ENOMEM;
        return NULL;
    }

    rc = libusb_init(&(x52_dev->ctx));
    if (rc) {
        errno = ELIBACC;
        goto err_recovery;
    }
    libusb_set_debug(x52_dev->ctx, 3);

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
                break;
            }
        }
    }
    libusb_free_device_list(list, 1);

    /* Make sure we actually have an X52 device detected */
    if (!x52_dev->hdl) {
        goto err_recovery;
    }

    return x52_dev;
err_recovery:
    free(x52_dev);
    return NULL;
}

void libx52_exit(libx52_device *dev)
{
    libusb_close(dev->hdl);
    libusb_exit(dev->ctx);
    free(dev);
}

