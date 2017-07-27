/*
 * LibUSB stub driver for testing the Saitek X52/X52 Pro
 *
 * Copyright (C) 2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#include <libusb-1.0/libusb.h>


int libusb_init(libusb_context **ctx)
{
    // TODO
    return 0;
}

void libusb_exit(libusb_context *ctx)
{
    // TODO
}

void libusb_set_debug(libusb_context *ctx, int level)
{
    // TODO
}

ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
    // TODO
    return 0;
}

void libusb_free_device_list(libusb_device **list, int unref_devices)
{
    // TODO
}

int libusb_get_device_descriptor(libusb_device *dev,
                                 struct libusb_device_descriptor *desc)
{
    // TODO
    return 0;
}

int libusb_open(libusb_device *dev, libusb_device_handle **handle)
{
    // TODO
    return 0;
}

void libusb_close(libusb_device_handle *dev_handle)
{
    // TODO
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
    // TODO
    return 0;
}
