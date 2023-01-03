/*
 * LibUSB test utility library
 *
 * This program calls the libusb_control_transfer API to log the bytes to
 * the given file. This is useful to generate a file with the same syntax
 * and verify that they match.
 *
 * Copyright (C) Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "libusbx52.h"
#include "x52_commands.h"

libusb_context *global_context;

static int send_command(libusb_device_handle *hdl, uint16_t index, uint16_t value)
{
    return libusb_control_transfer(hdl,
        LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
        X52_VENDOR_REQUEST, value, index, NULL, 0, 5000);
}

static libusb_device_handle *libusbx52_init(void)
{
    int rc;
    ssize_t count;
    int i;
    libusb_device dummy = { 0 };
    libusb_device *dummy_list[] = { &dummy };
    libusb_device **list = dummy_list;
    libusb_device_handle *hdl = NULL;
    struct libusb_device_descriptor desc;

    rc = libusb_init(&global_context);
    if (rc) {
        return NULL;
    }

    #if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106)
    /*
     * Use the libusb_set_option flag instead of libusb_set_debug. This
     * was introduced in libusb 1.0.22
     */
    libusb_set_option(global_context, LIBUSB_OPTION_LOG_LEVEL,
                      LIBUSB_LOG_LEVEL_ERROR);
    #else
    libusb_set_debug(global_context, LIBUSB_LOG_LEVEL_ERROR);
    #endif

    count = libusb_get_device_list(global_context, &list);
    for (i = 0; i < count; i++) {
        libusb_device *device = list[i];
        if (!libusb_get_device_descriptor(device, &desc)) {
            if (desc.idVendor == 0x06a3) {
                if (desc.idProduct == 0x0762) {
                    rc = libusb_open(device, &hdl);
                    if (rc) {
                        if (hdl) free(hdl);
                        hdl = NULL;
                        break;
                    }
                }
            }
        }
    }

    libusb_free_device_list(list, 1);

    return hdl;
}

int main(int argc, char *argv[])
{
    unsigned int index;
    unsigned int value;
    int i;
    libusb_device_handle *hdl;
    libusb_context *ctx;

    hdl = libusbx52_init();
    if (hdl == NULL) {
        return 1;
    }
    ctx = hdl->ctx;

    /* Process arguments until there are fewer than 2 remaining */
    for (i = 1; i < argc && (argc - i) >= 2; i += 2) {
        int parsed;
        parsed = sscanf(argv[i], "%x", &index);
        if (parsed != 1) break;

        parsed = sscanf(argv[i+1], "%x", &value);
        if (parsed != 1) break;

        send_command(hdl, index, value);
    }

    libusb_close(hdl);
    libusb_exit(ctx);

    return 0;
}
