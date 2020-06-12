/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "libx52.h"
#include "x52_commands.h"
#include "x52_common.h"

/* Translate a libusb error to a libx52 error */
int _x52_translate_libusb_error(enum libusb_error errcode)
{
    switch (errcode) {
    case LIBUSB_SUCCESS:
        return LIBX52_SUCCESS;

    case LIBUSB_ERROR_IO:
        return LIBX52_ERROR_IO;

    case LIBUSB_ERROR_INVALID_PARAM:
        return LIBX52_ERROR_INVALID_PARAM;

    case LIBUSB_ERROR_ACCESS:
        return LIBX52_ERROR_PERM;

    case LIBUSB_ERROR_NO_DEVICE:
        return LIBX52_ERROR_NO_DEVICE;

    case LIBUSB_ERROR_NOT_FOUND:
        return LIBX52_ERROR_NOT_FOUND;

    case LIBUSB_ERROR_BUSY:
        return LIBX52_ERROR_BUSY;

    case LIBUSB_ERROR_TIMEOUT:
        return LIBX52_ERROR_TIMEOUT;

    case LIBUSB_ERROR_OVERFLOW:
        return LIBX52_ERROR_OVERFLOW;

    case LIBUSB_ERROR_PIPE:
        return LIBX52_ERROR_PIPE;

    case LIBUSB_ERROR_INTERRUPTED:
        return LIBX52_ERROR_INTERRUPTED;

    case LIBUSB_ERROR_NO_MEM:
        return LIBX52_ERROR_OUT_OF_MEMORY;

    case LIBUSB_ERROR_NOT_SUPPORTED:
        return LIBX52_ERROR_NOT_SUPPORTED;

    case LIBUSB_ERROR_OTHER:
    default:
        return LIBX52_ERROR_USB_FAILURE;
    };
}

int _x52_vendor_command(libx52_device *x52, uint16_t index, uint16_t value)
{
    int j;
    int rc = 0;

    /* Allow retry in case of failure */
    for (j = 0; j < 3; j++) {
        rc = libusb_control_transfer(x52->hdl,
            LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
            X52_VENDOR_REQUEST, value, index, NULL, 0, 5000);

        if (rc == LIBUSB_SUCCESS) {
            break;
        }
    }

    return rc;
}

int libx52_vendor_command(libx52_device *x52, uint16_t index, uint16_t value)
{
    int rc = 0;

    /* It is possible for the vendor command to be called when the joystick
     * is not connected. Check for this and return an appropriate error.
     */
    if (!x52->hdl || !x52->vendor_cmd_fn) {
        return LIBX52_ERROR_NO_DEVICE;
    }

    rc = (*(x52->vendor_cmd_fn))(x52, index, value);

    /* Handle device removal */
    if (rc == LIBUSB_ERROR_NO_DEVICE) {
        /* Physical device has likely been disconnected, disconnect the virtual
         * handle, and report the failure.
         */
        (void)libx52_disconnect(x52);
    }

    return _x52_translate_libusb_error(rc);
}

static int _x52_write_shift(libx52_device *x52, uint32_t bit)
{
    uint16_t value;
    value = tst_bit(&x52->led_mask, X52_BIT_SHIFT) ? X52_SHIFT_ON : X52_SHIFT_OFF;
    return libx52_vendor_command(x52, X52_SHIFT_INDICATOR, value);
}

static int _x52_write_led(libx52_device *x52, uint32_t bit)
{
    uint16_t value;
    /* The bits correspond exactly to the LED identifiers */
    value = tst_bit(&x52->led_mask, bit) ? 1 : 0;
    return libx52_vendor_command(x52, X52_LED, value | (bit << 8));
}

static int _x52_write_line(libx52_device *x52, uint32_t bit)
{
    uint8_t i;
    uint8_t line_index = bit - X52_BIT_MFD_LINE1;
    int rc;

    const uint16_t line_index_map[X52_MFD_LINES] = {
        X52_MFD_LINE1,
        X52_MFD_LINE2,
        X52_MFD_LINE3,
    };

    /* Clear the line first */
    rc = libx52_vendor_command(x52,
            line_index_map[line_index] | X52_MFD_CLEAR_LINE, 0);
    if (rc) {
        return rc;
    }

    for (i = 0; i < x52->line[line_index].length; i += 2) {
        uint16_t value;
        value = x52->line[line_index].text[i + 1] << 8 |
                x52->line[line_index].text[i];

        rc = libx52_vendor_command(x52,
                line_index_map[line_index] | X52_MFD_WRITE_LINE, value);
        if (rc) {
            return rc;
        }
    }

    return rc;
}

static int _x52_write_pov_blink(libx52_device *x52, uint32_t bit)
{
    uint16_t value;
    value = tst_bit(&x52->led_mask, X52_BIT_POV_BLINK) ? X52_BLINK_ON : X52_BLINK_OFF;
    return libx52_vendor_command(x52, X52_BLINK_INDICATOR, value);
}

static int _x52_write_brightness(libx52_device *x52, uint32_t bit)
{
    uint16_t index;
    uint16_t value;

    if (bit == X52_BIT_BRI_MFD) {
        index = X52_MFD_BRIGHTNESS;
        value = x52->mfd_brightness;
    } else {
        index = X52_LED_BRIGHTNESS;
        value = x52->led_brightness;
    }

    return libx52_vendor_command(x52, index, value);
}

static int _x52_write_date(libx52_device *x52, uint32_t bit)
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
        return LIBX52_ERROR_INVALID_PARAM;
    }

    rc = libx52_vendor_command(x52, X52_DATE_DDMM, value1);
    if (rc == LIBX52_SUCCESS) {
        rc = libx52_vendor_command(x52, X52_DATE_YEAR, value2);
    }

    return rc;
}

uint16_t libx52_calculate_clock_offset(libx52_device *x52, libx52_clock_id clock, uint16_t h24)
{
    int offset;
    int negative;

    offset = x52->timezone[clock] - x52->timezone[LIBX52_CLOCK_1];

    /* Save the preliminary state, if negative, set the negative flag */
    if (offset < 0) {
        negative = 1;
        offset = -offset;
    } else {
        negative = 0;
    }

    /* Because the packet format limits the offset from the base clock to
     * a maximum of +/- 1023, we can only handle a maximum offset of 17
     * hours from the base clock. Eg. Honolulu and Auckland at -1000 and
     * +1200 respectively have a direct offset of +22 hours. A potentially
     * greater issue is to take the two extreme timezones at -1200 and +1400
     * for a difference of +26 hours. The safe bet is to check if the offset
     * exceeds +/- 1023, and subtract 1440, which will convert the offset of
     * +22 to -2 hours, or +26 to +2 hours.
     */
    while (offset > 1023) {
        offset -= 1440; // Subtract 24 hours
    }

    /* If offset is between 1024 and 1440, then we need to reverse the
     * negative flag
     */
    if (offset < 0) {
        negative = !negative;
        offset = -offset;
    }

    return (h24 << 15 | negative << 10 | (offset & 0x3FF));
}

static int _x52_write_time(libx52_device *x52, uint32_t bit)
{
    uint16_t value = 0;
    uint16_t index;
    libx52_clock_id clock;
    uint16_t h24;

    switch (bit) {
    case X52_BIT_MFD_TIME:
        clock = LIBX52_CLOCK_1;
        index = X52_TIME_CLOCK1;
        break;
    case X52_BIT_MFD_OFFS1:
        clock = LIBX52_CLOCK_2;
        index = X52_OFFS_CLOCK2;
        break;
    case X52_BIT_MFD_OFFS2:
        clock = LIBX52_CLOCK_3;
        index = X52_OFFS_CLOCK3;
        break;
    default:
        /* We should never get here, but put in a dummy case to satisfy the compiler */
        return LIBX52_ERROR_INVALID_PARAM;
    }

    h24 = !!(x52->time_format[clock]);

    if (clock != LIBX52_CLOCK_1) {
        value = libx52_calculate_clock_offset(x52, clock, h24);
    } else {
        value = h24 << 15 |
                (x52->time_hour & 0x7F) << 8 |
                (x52->time_minute & 0xFF);
    }

    return libx52_vendor_command(x52, index, value);
}

typedef int (*x52_handler)(libx52_device *, uint32_t);

const x52_handler _x52_handlers[32] = {
    [X52_BIT_SHIFT]         = _x52_write_shift,
    [X52_BIT_LED_FIRE]      = _x52_write_led,
    [X52_BIT_LED_A_RED]     = _x52_write_led,
    [X52_BIT_LED_A_GREEN]   = _x52_write_led,
    [X52_BIT_LED_B_RED]     = _x52_write_led,
    [X52_BIT_LED_B_GREEN]   = _x52_write_led,
    [X52_BIT_LED_D_RED]     = _x52_write_led,
    [X52_BIT_LED_D_GREEN]   = _x52_write_led,
    [X52_BIT_LED_E_RED]     = _x52_write_led,
    [X52_BIT_LED_E_GREEN]   = _x52_write_led,
    [X52_BIT_LED_T1_RED]    = _x52_write_led,
    [X52_BIT_LED_T1_GREEN]  = _x52_write_led,
    [X52_BIT_LED_T2_RED]    = _x52_write_led,
    [X52_BIT_LED_T2_GREEN]  = _x52_write_led,
    [X52_BIT_LED_T3_RED]    = _x52_write_led,
    [X52_BIT_LED_T3_GREEN]  = _x52_write_led,
    [X52_BIT_LED_POV_RED]   = _x52_write_led,
    [X52_BIT_LED_POV_GREEN] = _x52_write_led,
    [X52_BIT_LED_I_RED]     = _x52_write_led,
    [X52_BIT_LED_I_GREEN]   = _x52_write_led,
    [X52_BIT_LED_THROTTLE]  = _x52_write_led,
    [X52_BIT_MFD_LINE1]     = _x52_write_line,
    [X52_BIT_MFD_LINE2]     = _x52_write_line,
    [X52_BIT_MFD_LINE3]     = _x52_write_line,
    [X52_BIT_POV_BLINK]     = _x52_write_pov_blink,
    [X52_BIT_BRI_MFD]       = _x52_write_brightness,
    [X52_BIT_BRI_LED]       = _x52_write_brightness,
    [X52_BIT_MFD_DATE]      = _x52_write_date,
    [X52_BIT_MFD_TIME]      = _x52_write_time,
    [X52_BIT_MFD_OFFS1]     = _x52_write_time,
    [X52_BIT_MFD_OFFS2]     = _x52_write_time,
};

int libx52_update(libx52_device *x52)
{
    unsigned int i;
    uint32_t update_mask;
    int rc = LIBX52_SUCCESS;
    x52_handler handler;

    /* It is possible for the update command to be called when the joystick
     * is not connected. Check for this and return an appropriate error.
     */
    if (!x52->hdl) {
        return LIBX52_ERROR_NO_DEVICE;
    }

    /* Save the update mask */
    update_mask = x52->update_mask;
    /* Reset the device update mask to 0 */
    x52->update_mask = 0;

    for (i = 0; i < 32; i++) {
        if (tst_bit(&update_mask, i)) {
            rc = LIBX52_SUCCESS;
            handler = _x52_handlers[i];
            if (handler != NULL) {
                rc = (*handler)(x52, i);
            }

            if (rc == LIBX52_SUCCESS) {
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
