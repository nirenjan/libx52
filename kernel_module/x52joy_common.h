/*
 * Saitek X52 Pro HOTAS driver
 *
 * Copyright (C) 2012 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef X52JOY_COMMON_H
#define X52JOY_COMMON_H

#include <linux/usb.h>
#include <linux/input.h>

#include "x52joy_map.h"

struct x52_joy {
    struct usb_device   *udev;
    struct urb          *irq_in;
    unsigned char       *idata;
    dma_addr_t          idata_dma;
    struct input_dev    *idev;
    char                phys[64];

    u32                 led_status;
    struct x52_mfd_line line[X52_MFD_LINES];
    struct x52_mfd_date date;
    struct x52_mfd_time time;
    struct x52_mfd_offs time_offs2;
    struct x52_mfd_offs time_offs3;

    u8                  type;
    u8                  bri_mfd;
    u8                  bri_led;

    u8                  shift_ind:1;
    u8                  blink_led:1;
    u8                  clutch_mode:1;

    u8                  shift_state_enabled:1;
    u8                  clock_enabled:1;
    u8                  clutch_enabled:1;
    u8                  clutch_latched:1;
    u8                  :1;

    u8                  feat_mfd:1;
    u8                  feat_led:1;
    u8                  debug:1;
    u8                  :5;
};

#define X52_PACKET_LEN      16

#define X52TYPE_X52         1
#define X52TYPE_X52PRO      2
#define X52TYPE_UNKNOWN     0

#define VENDOR_ID_SAITEK    0x06a3
#define PRODUCT_ID_X52_PRO  0x0762

#define X52FLAGS_SUPPORTS_MFD   (1 << 0)
#define X52FLAGS_SUPPORTS_LED   (1 << 1)

int set_text(struct x52_joy *joy, u8 line_no);
int set_brightness(struct x52_joy *joy, u8 target);
int set_led(struct x52_joy *joy, u8 target);
int set_date(struct x52_joy *joy);
int set_shift(struct x52_joy *joy);
int set_blink(struct x52_joy *joy);

void x52_irq_handler(struct urb *urb);
void x52_setup_input(struct input_dev *idev);
int x52_open (struct input_dev *idev);
void x52_close (struct input_dev *idev);

#endif /* !defined X52JOY_COMMON_H */
