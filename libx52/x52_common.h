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

#ifndef X52JOY_COMMON_H
#define X52JOY_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <libusb.h>

/*
 * The X52 MFD supports the following:
 *  - 3 lines of 16 characters each
 *  - Clock with HH:MM
 *  - Date with YYMMDD (IIRC)
 */
#define X52_MFD_LINE_SIZE   16
#define X52_MFD_LINES       3

struct x52_mfd_line {
    uint8_t     text[X52_MFD_LINE_SIZE];
    uint8_t     length;
};

enum x52_mfd_date_format {
    x52_mfd_format_yymmdd,  /* YY-MM-DD */
    x52_mfd_format_mmddyy,  /* MM-DD-YY */
    x52_mfd_format_ddmmyy,  /* DD-MM-YY */
    x52_mfd_format_max,
};

struct x52_mfd_date {
    uint8_t     year;
    uint8_t     month;
    uint8_t     day;
    uint8_t     format;     /* See format enum */
};

struct x52_mfd_time {
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     h24;        /* 24 hour format if 1 */
};

struct x52_mfd_offs {
    uint16_t    min_off;    /* Minute offset from clock 0 */
    uint8_t     neg_off;    /* Negative offset if 1 */
    uint8_t     h24;        /* 24 hour format if 1 */
};

struct libx52_device {
    libusb_context *ctx;
    libusb_device_handle *hdl;
    struct libusb_transfer *in_xfer;
    struct libusb_transfer *ctrl_xfer;

    uint32_t update_mask;
    uint8_t  input_buffer[16];

    uint32_t led_mask;
    uint16_t mfd_brightness;
    uint16_t led_brightness;

    struct x52_mfd_line line[X52_MFD_LINES];
    struct x52_mfd_date date;
    struct x52_mfd_time time;
    struct x52_mfd_offs offs[2];
};

/** Indicator bits for update mask */
#define X52_BIT_SHIFT           0
#define X52_BIT_LED_FIRE        1
#define X52_BIT_LED_A_RED       2
#define X52_BIT_LED_A_GREEN     3
#define X52_BIT_LED_B_RED       4
#define X52_BIT_LED_B_GREEN     5
#define X52_BIT_LED_D_RED       6
#define X52_BIT_LED_D_GREEN     7
#define X52_BIT_LED_E_RED       8
#define X52_BIT_LED_E_GREEN     9
#define X52_BIT_LED_T1_RED      10
#define X52_BIT_LED_T1_GREEN    11
#define X52_BIT_LED_T2_RED      12
#define X52_BIT_LED_T2_GREEN    13
#define X52_BIT_LED_T3_RED      14
#define X52_BIT_LED_T3_GREEN    15
#define X52_BIT_LED_POV_RED     16
#define X52_BIT_LED_POV_GREEN   17
#define X52_BIT_LED_I_RED       18
#define X52_BIT_LED_I_GREEN     19
#define X52_BIT_LED_THROTTLE    20
#define X52_BIT_MFD_LINE1       21
#define X52_BIT_MFD_LINE2       22
#define X52_BIT_MFD_LINE3       23
#define X52_BIT_POV_BLINK       24
#define X52_BIT_BRI_MFD         25
#define X52_BIT_BRI_LED         26
#define X52_BIT_MFD_DATE        27
#define X52_BIT_MFD_TIME        28
#define X52_BIT_MFD_OFFS1       29
#define X52_BIT_MFD_OFFS2       30

#endif /* !defined X52JOY_COMMON_H */
