/*
 * libx52 bugreport utility
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/utsname.h>

#include "libusb.h"
#include "hidapi.h"
#include "libx52io.h"

static void print_sysinfo(void)
{
    struct utsname uts;

    puts("");
    puts("System info:");
    puts("============");
    if (uname(&uts) < 0) {
        printf("Unable to get system info: %s\n", strerror(errno));
    } else {
        printf("%s %s %s (%s)\n", uts.sysname, uts.release, uts.machine, uts.version);
    }
}

static void print_devinfo(void)
{
    libx52io_context *ctx;
    int rc;

    puts("");
    puts("Device info:");
    puts("============");
    #define CHECK_RC() do { \
        if (rc != LIBX52IO_SUCCESS) { \
            puts(libx52io_strerror(rc)); \
            return; \
        } \
    } while (0)

    rc = libx52io_init(&ctx);
    CHECK_RC();

    rc = libx52io_open(ctx);
    CHECK_RC();

    printf("Device ID: vendor 0x%04x product 0x%04x version 0x%04x\n",
           libx52io_get_vendor_id(ctx),
           libx52io_get_product_id(ctx),
           libx52io_get_device_version(ctx));
    printf("Device name: '%s' '%s'\n",
           libx52io_get_manufacturer_string(ctx),
           libx52io_get_product_string(ctx));
    printf("Serial number: '%s'\n",
           libx52io_get_serial_number_string(ctx));
}

int main(int argc, char **argv)
{
    const struct libusb_version *libusb;

    puts("libx52 bugreport");
    puts("================");
    printf("Package version: %s\n", VERSION);
    printf("Build version: %s\n", BUILD_VERSION);

    puts("");
    puts("Built against:");
    puts("==============");

    printf("libusb API version: 0x%08x\n", LIBUSB_API_VERSION);
#if defined HID_API_VERSION_STR
    printf("hidapi version: %s\n", HID_API_VERSION_STR);
#endif

    libusb = libusb_get_version();
    puts("");
    puts("System versions:");
    puts("================");
    printf("libusb: %d.%d.%d.%d%s (%s)\n",
           libusb->major, libusb->minor, libusb->micro, libusb->nano,
           libusb->rc, libusb->describe);
#if defined HID_API_VERSION_STR
    printf("hidapi: %s\n", hid_version_str());
#endif

    print_sysinfo();
    print_devinfo();

    return 0;
}
