/*
 * Saitek X52 Pro HOTAS driver
 *
 * Copyright (C) 2012 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#ifndef X52JOY_COMMON_H
#define X52JOY_COMMON_H

#include <linux/usb.h>

#include "x52joy_map.h"

struct x52_joy {
    struct usb_device   *udev;
    u32                 led_status;
    struct x52_mfd_line line[X52_MFD_LINES];
    struct x52_mfd_date date;
    struct x52_mfd_time time;
    struct x52_mfd_offs time_offs2;
    struct x52_mfd_offs time_offs3;

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

int set_text(struct x52_joy *joy, u8 line_no);
int set_brightness(struct x52_joy *joy, u8 target);
int set_led(struct x52_joy *joy, u8 target);
int set_date(struct x52_joy *joy);
int set_shift(struct x52_joy *joy);
int set_blink(struct x52_joy *joy);

#endif /* !defined X52JOY_COMMON_H */
