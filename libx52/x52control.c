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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libusb.h>

#include "libx52.h"
#include "x52control.h"
#include "x52_common.h"

static int libx52_control_transfer(libx52_device *x52, uint16_t index, uint16_t value)
{
    return libusb_control_transfer(x52->hdl,
            LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
            X52_VENDOR_REQUEST, value, index, NULL, 0, 1000);
}

inline void set_bit(uint32_t *value, uint32_t bit)
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

static int libx52_write_line(libx52_device *x52, uint8_t line_index)
{
    uint8_t i;
    uint16_t value;
    int rc;

    const uint16_t line_index_map[X52_MFD_LINES] = {
        X52_MFD_LINE1,
        X52_MFD_LINE2,
        X52_MFD_LINE3,
    };

    /* Clear the line first */
    rc = libx52_control_transfer(x52,
            line_index_map[line_index] | X52_MFD_CLEAR_LINE, 0);
    if (rc) {
        return rc;
    }

    for (i = 0; i < x52->line[line_index].length; i += 2) {
        value = x52->line[line_index].text[i + 1] << 8 |
                x52->line[line_index].text[i];

        rc = libx52_control_transfer(x52,
                line_index_map[line_index] | X52_MFD_WRITE_LINE, value);
        if (rc) {
            return rc;
        }
    }

    return rc;
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

    memset(x52->line[line].text, ' ', X52_MFD_LINE_SIZE);
    memcpy(x52->line[line].text, text, length);
    x52->line[line].length = length;
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

    return 0;
}

int libx52_set_time(libx52_device *x52, uint8_t hour, uint8_t minute, uint8_t format)
{
    if (!x52) {
        return -EINVAL;
    }

    x52->time.hour = hour;
    x52->time.minute = minute;
    x52->time.h24 = !!format;

    set_bit(&x52->update_mask, X52_BIT_MFD_TIME);

    return 0;
}

static int libx52_write_date(libx52_device *x52)
{
    uint16_t value1; //dd-mm
    uint16_t value2; //yy
    int rc;

    switch (x52->date.format) {
    case x52_mfd_format_yymmdd:
        value1 = x52->date.month << 8 |
                 x52->date.year;
        value2 = x52->date.day;
        break;

    case x52_mfd_format_mmddyy:
        value1 = x52->date.day << 8 |
                 x52->date.month;
        value2 = x52->date.year;
        break;

    case x52_mfd_format_ddmmyy:
        value1 = x52->date.month << 8 |
                 x52->date.day;
        value2 = x52->date.year;
        break;

    default:
        return -EINVAL;
    }

    rc = libx52_control_transfer(x52, X52_DATE_DDMM, value1);
    if (rc == 0) {
        rc = libx52_control_transfer(x52, X52_DATE_YEAR, value2);
    }

    return rc;
}

int libx52_set_brightness(libx52_device *x52, uint8_t mfd, uint16_t brightness)
{
    if (!x52) {
        return -EINVAL;
    }

    if (mfd) {
        x52->mfd_brightness = brightness;
        set_bit(&x52->update_mask, X52_BIT_BRI_MFD);
    } else {
        x52->led_brightness = brightness;
        set_bit(&x52->update_mask, X52_BIT_BRI_LED);
    }

    return 0;
}

int libx52_set_shift(libx52_device *x52, uint8_t state)
{
    if (!x52) {
        return -EINVAL;
    }

    if (state) {
        set_bit(&x52->led_mask, X52_BIT_SHIFT);
    } else {
        clr_bit(&x52->led_mask, X52_BIT_SHIFT);
    }

    set_bit(&x52->update_mask, X52_BIT_SHIFT);
    return 0;
}

int libx52_set_blink(libx52_device *x52, uint8_t state)
{
    if (!x52) {
        return -EINVAL;
    }

    if (state) {
        set_bit(&x52->led_mask, X52_BIT_POV_BLINK);
    } else {
        clr_bit(&x52->led_mask, X52_BIT_POV_BLINK);
    }

    set_bit(&x52->update_mask, X52_BIT_POV_BLINK);
    return 0;
}

int libx52_set_clock(libx52_device *x52, time_t time, int local)
{
    struct tm timeval;
    if (!x52) {
        return -EINVAL;
    }

    if (local) {
        timeval = *localtime(&time);
    } else {
        timeval = *gmtime(&time);
    }

    x52->date.day = timeval.tm_mday;
    x52->date.month = timeval.tm_mon + 1;
    x52->date.year = timeval.tm_year % 100;
    x52->time.hour = timeval.tm_hour;
    x52->time.minute = timeval.tm_min;

    set_bit(&x52->update_mask, X52_BIT_MFD_DATE);
    set_bit(&x52->update_mask, X52_BIT_MFD_TIME);
    return 0;
}

int libx52_update(libx52_device *x52)
{
    unsigned int i;
    uint32_t update_mask;
    uint16_t value;
    int rc;

    /* Save the update mask */
    update_mask = x52->update_mask;
    /* Reset the device update mask to 0 */
    x52->update_mask = 0;

    for (i = 0; i < 32; i++) {
        if (tst_bit(&update_mask, i)) {
            switch (i) {
            case X52_BIT_SHIFT:
                value = tst_bit(&x52->led_mask, X52_BIT_SHIFT) ? X52_SHIFT_ON : X52_SHIFT_OFF;
                rc = libx52_control_transfer(x52, X52_SHIFT_INDICATOR, value);
                break;

            case X52_BIT_LED_FIRE:
            case X52_BIT_LED_A_RED:
            case X52_BIT_LED_A_GREEN:
            case X52_BIT_LED_B_RED:
            case X52_BIT_LED_B_GREEN:
            case X52_BIT_LED_D_RED:
            case X52_BIT_LED_D_GREEN:
            case X52_BIT_LED_E_RED:
            case X52_BIT_LED_E_GREEN:
            case X52_BIT_LED_T1_RED:
            case X52_BIT_LED_T1_GREEN:
            case X52_BIT_LED_T2_RED:
            case X52_BIT_LED_T2_GREEN:
            case X52_BIT_LED_T3_RED:
            case X52_BIT_LED_T3_GREEN:
            case X52_BIT_LED_POV_RED:
            case X52_BIT_LED_POV_GREEN:
            case X52_BIT_LED_I_RED:
            case X52_BIT_LED_I_GREEN:
            case X52_BIT_LED_THROTTLE:
                /* The bits correspond exactly to the LED identifiers */
                value = tst_bit(&x52->led_mask, i) ? 1 : 0;
                rc = libx52_control_transfer(x52, X52_LED, value | (i << 8));
                break;

            case X52_BIT_MFD_LINE1:
                rc = libx52_write_line(x52, 0);
                break;

            case X52_BIT_MFD_LINE2:
                rc = libx52_write_line(x52, 1);
                break;

            case X52_BIT_MFD_LINE3:
                rc = libx52_write_line(x52, 2);
                break;

            case X52_BIT_POV_BLINK:
                value = tst_bit(&x52->led_mask, X52_BIT_POV_BLINK) ? X52_BLINK_ON : X52_BLINK_OFF;
                rc = libx52_control_transfer(x52, X52_BLINK_INDICATOR, value);
                break;

            case X52_BIT_BRI_MFD:
                rc = libx52_control_transfer(x52, X52_MFD_BRIGHTNESS,
                        x52->mfd_brightness);
                break;

            case X52_BIT_BRI_LED:
                rc = libx52_control_transfer(x52, X52_LED_BRIGHTNESS,
                        x52->led_brightness);
                break;

            case X52_BIT_MFD_DATE:
                rc = libx52_write_date(x52);
                break;

            case X52_BIT_MFD_TIME:
                value = x52->time.h24 << 15 |
                        x52->time.hour << 8 |
                        x52->time.minute;
                rc = libx52_control_transfer(x52, X52_TIME_CLOCK1, value);
                break;

            case X52_BIT_MFD_OFFS1:
            case X52_BIT_MFD_OFFS2:
            default:
                /* Ignore any spurious bits */
                rc = 0;
                break;
            }

            if (rc == 0) {
                clr_bit(&update_mask, i);
            } else {
                /* Last transfer failed - reset the update mask */
                x52->update_mask = update_mask;
                break;
            }
        }
    }

    return rc;
}
