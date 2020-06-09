/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52JOY_COMMON_H
#define X52JOY_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <libusb.h>
#include "libx52.h"

/*
 * The X52 MFD supports the following:
 *  - 3 lines of 16 characters each
 *  - Base clock with HH:MM [AM/PM]
 *  - Date with YYMMDD (IIRC)
 *  - 2 additional clocks offset from base clock
 */
#define X52_MFD_LINE_SIZE   16
#define X52_MFD_LINES       3
#define X52_MFD_CLOCKS      3

struct x52_mfd_line {
    uint8_t     text[X52_MFD_LINE_SIZE];
    uint8_t     length;
};

struct libx52_device {
    libusb_context *ctx;
    libusb_device_handle *hdl;

    uint32_t update_mask;
    uint32_t flags;

    uint32_t led_mask;
    uint16_t mfd_brightness;
    uint16_t led_brightness;

    struct x52_mfd_line line[X52_MFD_LINES];
    libx52_date_format date_format;
    int date_day;
    int date_month;
    int date_year;
    int time_hour;
    int time_minute;

    int timezone[X52_MFD_CLOCKS];
    libx52_clock_format time_format[X52_MFD_CLOCKS];
};

/** Flag bits */
#define X52_FLAG_IS_PRO         0

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

static inline void set_bit(uint32_t *value, uint32_t bit)
{
    *value |= (1UL << bit);
}

static inline void clr_bit(uint32_t *value, uint32_t bit)
{
    *value &= ~(1UL << bit);
}

static inline uint32_t tst_bit(uint32_t *value, uint32_t bit)
{
    return (*value & (1UL << bit));
}

int _x52_translate_libusb_error(enum libusb_error errcode);
uint16_t libx52_calculate_clock_offset(libx52_device *x52, libx52_clock_id clock, uint16_t h24);

#endif /* !defined X52JOY_COMMON_H */
