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
#include "x52_commands.h"
#include "x52_common.h"

static int libx52_control_transfer(libx52_device *x52, uint16_t index, uint16_t value)
{
    return libusb_control_transfer(x52->hdl,
            LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
            X52_VENDOR_REQUEST, value, index, NULL, 0, 1000);
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

static int libx52_write_date(libx52_device *x52)
{
    uint16_t value1; //dd-mm
    uint16_t value2; //yy
    int rc;

    switch (x52->date_format) {
    case LIBX52_DATE_FORMAT_YYMMDD:
        value1 = x52->date_month << 8 |
                 x52->date_year;
        value2 = x52->date_day;
        break;

    case LIBX52_DATE_FORMAT_MMDDYY:
        value1 = x52->date_day << 8 |
                 x52->date_month;
        value2 = x52->date_year;
        break;

    case LIBX52_DATE_FORMAT_DDMMYY:
        value1 = x52->date_month << 8 |
                 x52->date_day;
        value2 = x52->date_year;
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
