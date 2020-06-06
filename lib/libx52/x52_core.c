/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "libx52.h"
#include "x52_commands.h"
#include "x52_common.h"
#include "gettext.h"

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

int libx52_disconnect(libx52_device *dev)
{
    if (!dev) {
       return LIBX52_ERROR_INVALID_PARAM;
    }

    if (dev->hdl) {
        libusb_close(dev->hdl);
        dev->hdl = NULL;
        dev->flags = 0;
    }

    return LIBX52_SUCCESS;
}

int libx52_connect(libx52_device *dev)
{
    int rc;
    ssize_t count;
    int i;
    libusb_device **list;
    libusb_device_handle *hdl;
    struct libusb_device_descriptor desc;

    /* Make sure that we have a valid pointer */
    if (!dev) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Disconnect any existing handles. This will force libx52 to rescan the
     * device list and bind to the first supported joystick, if any. If the
     * joystick was unplugged between subsequent calls to this function, then
     * it will return a No device error. This also means that a new device
     * handle is cached in the device structure.
     */
    (void)libx52_disconnect(dev);

    count = libusb_get_device_list(dev->ctx, &list);
    for (i = 0; i < count; i++) {
        libusb_device *device;

        device = list[i];
        if (!libusb_get_device_descriptor(device, &desc)) {
            if (libx52_check_product(desc.idVendor, desc.idProduct)) {
                rc = libusb_open(device, &hdl);
                if (rc) {
                    return _x52_translate_libusb_error(rc);
                }

                dev->hdl = hdl;

                if (libx52_device_is_x52pro(desc.idProduct)) {
                    set_bit(&(dev->flags), X52_FLAG_IS_PRO);
                }
                break;
            }
        }
    }
    libusb_free_device_list(list, 1);

    /* Make sure we actually have an X52 device detected */
    if (!dev->hdl) {
        return LIBX52_ERROR_NO_DEVICE;
    }

    return LIBX52_SUCCESS;
}

int libx52_init(libx52_device **dev)
{
    int rc;
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
        free(x52_dev);
        return LIBX52_ERROR_INIT_FAILURE;
    }

    #if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106)
    /*
     * Use the libusb_set_option flag instead of libusb_set_debug. This
     * was introduced in libusb 1.0.22
     */
    libusb_set_option(x52_dev->ctx, LIBUSB_OPTION_LOG_LEVEL,
                      LIBUSB_LOG_LEVEL_WARNING);
    #else
    libusb_set_debug(x52_dev->ctx, LIBUSB_LOG_LEVEL_WARNING);
    #endif

    /* Try to connect to any supported joystick. It's OK if there aren't
     * any available to connect to, subsequent calls to libx52_connect will
     * be used to open the device handle
     */
    (void)libx52_connect(x52_dev);

    *dev = x52_dev;

    /* Setup the gettext utilities */
    bindtextdomain(PACKAGE, LOCALEDIR);

    return LIBX52_SUCCESS;
}

void libx52_exit(libx52_device *dev)
{
    libx52_disconnect(dev);
    libusb_exit(dev->ctx);

    /* Clear the memory to prevent reuse */
    memset(dev, 0, sizeof(*dev));

    free(dev);
}

int libx52_check_feature(libx52_device *dev, libx52_feature feature)
{
    if (!dev) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    switch (feature) {
    case LIBX52_FEATURE_LED:
        return tst_bit(&(dev->flags), X52_FLAG_IS_PRO) ?
            LIBX52_SUCCESS :
            LIBX52_ERROR_NOT_SUPPORTED;
    }

    return LIBX52_ERROR_INVALID_PARAM;
}
