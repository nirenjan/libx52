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

/*
 * The X52 MFD supports the following:
 *  - 3 lines of 16 characters each
 *  - Clock with HH:MM
 *  - Date with YYMMDD (IIRC)
 */
#define X52_MFD_LINE_SIZE   16
#define X52_MFD_LINES       3

struct x52_mfd_line {
    u8      text[X52_MFD_LINE_SIZE];
    u8      length;
};

enum x52_mfd_date_format {
    x52_mfd_format_yymmdd,  /* YY-MM-DD */
    x52_mfd_format_mmddyy,  /* MM-DD-YY */
    x52_mfd_format_ddmmyy,  /* DD-MM-YY */
    x52_mfd_format_max,
};

struct x52_mfd_date {
    u8      year;
    u8      month;
    u8      day;
    u8      format;     /* See format enum */
};

struct x52_mfd_time {
    u8      hour;
    u8      minute;
    u8      h24;        /* 24 hour format if 1 */
};

struct x52_mfd_offs {
    u8      min_off;    /* Minute offset from clock 0 */
    u8      neg_off;    /* Negative offset if 1 */
    u8      h24;        /* 24 hour format if 1 */
};

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
