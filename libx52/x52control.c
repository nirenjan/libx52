/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012 Nirenjan Krishnan (nirenjan@nirenjan.org)
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

#include <libusb.h>

#include "libx52.h"
#include "x52control.h"
#include "x52_common.h"

static libusb_context *libx52_ctx;

#define VENDOR_SAITEK 0x06a3
#define X52_PROD_X52PRO 0x0762

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

inline void set_bit(uint32_t *value, int bit)
{
    *value |= (1UL << bit);
}

inline void clr_bit(uint32_t *value, uint32_t bit)
{
    *value &= ~(1UL << bit);
}

inline uint32_t tst_bit(uint32_t *value, uint32_t bit)
{
    return (*value & (1UL << bit));
}

int libx52_set_text(libx52_device *x52, uint8_t line, const char *text, uint8_t length)
{
    if (!x52 || !text) {
        return -EINVAL;
    }

    if (line > 2) {
        return -EINVAL;
    }

    if (length > X52_MFD_LINE_SIZE) {
        length = X52_MFD_LINE_SIZE;
    }

    memcpy(x52->line[line].text, text, length);
    set_bit(&x52->update_mask, X52_BIT_MFD_LINE1 + line);

    return 0;
}

int libx52_set_led(libx52_device *x52, uint8_t led, uint8_t state)
{
    if (!x52) {
        return -EINVAL;
    }

    if (led > X52_BIT_LED_THROTTLE || led < X52_BIT_LED_FIRE) {
        return -EINVAL;
    }

    if (state) {
        set_bit(&x52->led_mask, led);
    } else {
        clr_bit(&x52->led_mask, led);
    }

    set_bit(&x52->update_mask, led);
    return 0;
}

int libx52_set_date(libx52_device *x52, uint8_t date, uint8_t month, uint8_t year, uint8_t format)
{
    if (!x52 || format >= x52_mfd_format_max) {
        return -EINVAL;
    }

    x52->date.day = date;
    x52->date.month = month;
    x52->date.year = year;
    x52->date.format = format;

    set_bit(&x52->update_mask, X52_BIT_MFD_DATE);
}

int libx52_set_time(libx52_device *x52, uint8_t hour, uint8_t minute, uint8_t format)
{
    if (!x52 || format >= x52_mfd_format_max) {
        return -EINVAL;
    }

    x52->time.hour = hour;
    x52->time.minute = minute;
    x52->time.h24 = format;

    set_bit(&x52->update_mask, X52_BIT_MFD_TIME);
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
    char pname[64];
    libx52_device *x52_dev;

    rc = libusb_init(&libx52_ctx);
    if (rc) {
        fprintf(stderr, "Unable to initialize libusb; rc=%d\n", rc);
        return NULL;
    }
    libusb_set_debug(libx52_ctx, 3);

    count = libusb_get_device_list(libx52_ctx, &list);
    for (i = 0; i < count; i++) {
        device = list[i];
        printf("Device %d:\n", i);
        printf("\tBus: %d\n", libusb_get_bus_number(device));
        printf("\tAddress: %d\n", libusb_get_device_address(device));
        printf("\tSpeed: %d\n", libusb_get_device_speed(device));

        if (!libusb_get_device_descriptor(device, &desc)) {
            printf("\tVID/PID: %04x %04x\n", desc.idVendor, desc.idProduct);

            if (libx52_check_product(desc.idVendor, desc.idProduct)) {
                x52_dev = malloc(sizeof(libx52_device));
                if (!x52_dev) {
                    fprintf(stderr, "Cannot allocate libx52_device\n");
                    return NULL;
                }

                rc = libusb_open(device, &hdl);
                if (rc) {
                    free(x52_dev);
                }

                x52_dev->hdl = hdl;
            
            if(libusb_get_string_descriptor_ascii(hdl, desc.iManufacturer, pname, 64) > 0 || 1) {
                printf("\tManufacturer: %s\n", pname);
            }

            if(libusb_get_string_descriptor_ascii(hdl, desc.iProduct, pname, 64) > 0 || 1) {
                printf("\tProduct: %s\n", pname);
            }
            }
        }
        printf("\n");
    }
    libusb_free_device_list(list, 1);

    return x52_dev;
}

void libx52_exit(libx52_device *dev)
{
    libusb_close(dev->hdl);
    free(dev);
    libusb_exit(libx52_ctx);
}

int main()
{
    int rc;
    ssize_t count;
    int i;
    libx52_device *dev;
    
    dev = libx52_init();
    libx52_exit(dev);
    return 0;
}
