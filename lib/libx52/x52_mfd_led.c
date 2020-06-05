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

#include "x52_common.h"

int libx52_set_text(libx52_device *x52, uint8_t line, const char *text, uint8_t length)
{
    if (!x52 || !text) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (line > 2) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (length > X52_MFD_LINE_SIZE) {
        length = X52_MFD_LINE_SIZE;
    }

    memset(x52->line[line].text, ' ', X52_MFD_LINE_SIZE);
    memcpy(x52->line[line].text, text, length);
    x52->line[line].length = length;
    set_bit(&x52->update_mask, X52_BIT_MFD_LINE1 + line);

    return LIBX52_SUCCESS;
}

static int x52pro_set_led_state(libx52_device *x52, libx52_led_id led, libx52_led_state state)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    switch (led) {
    case LIBX52_LED_FIRE:
    case LIBX52_LED_THROTTLE:
        if (state == LIBX52_LED_STATE_OFF) {
            clr_bit(&x52->led_mask, led);
            set_bit(&x52->update_mask, led);
        } else if (state == LIBX52_LED_STATE_ON) {
            set_bit(&x52->led_mask, led);
            set_bit(&x52->update_mask, led);
        } else {
            /* Colors not supported */
            return LIBX52_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        /* All other LEDs support OFF, RED, AMBER and GREEN states
         * However, they are composed of individual RED and GREEN LEDs which
         * must be turned on or off individually.
         */
        switch (state) {
        case LIBX52_LED_STATE_OFF:
            clr_bit(&x52->led_mask, led + 0); // Red
            clr_bit(&x52->led_mask, led + 1); // Green
            break;

        case LIBX52_LED_STATE_RED:
            set_bit(&x52->led_mask, led + 0); // Red
            clr_bit(&x52->led_mask, led + 1); // Green
            break;

        case LIBX52_LED_STATE_AMBER:
            set_bit(&x52->led_mask, led + 0); // Red
            set_bit(&x52->led_mask, led + 1); // Green
            break;

        case LIBX52_LED_STATE_GREEN:
            clr_bit(&x52->led_mask, led + 0); // Red
            set_bit(&x52->led_mask, led + 1); // Green
            break;

        default:
            /* Cannot set the LED to "ON" */
            return LIBX52_ERROR_NOT_SUPPORTED;
        }

        /* Set the update mask bits */
        set_bit(&x52->update_mask, led + 0); // Red
        set_bit(&x52->update_mask, led + 1); // Green
        break;
    }

    return LIBX52_SUCCESS;
}

int libx52_set_led_state(libx52_device *x52, libx52_led_id led, libx52_led_state state)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (tst_bit(&x52->flags, X52_FLAG_IS_PRO)) {
        return x52pro_set_led_state(x52, led, state);
    }

    /*
     * For now, we only support setting the LEDs on the X52 Pro model.
     * Calling this API on a non-Pro model will return a not supported error.
     */
    return LIBX52_ERROR_NOT_SUPPORTED;
}

int libx52_set_brightness(libx52_device *x52, uint8_t mfd, uint16_t brightness)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (mfd) {
        x52->mfd_brightness = brightness;
        set_bit(&x52->update_mask, X52_BIT_BRI_MFD);
    } else {
        x52->led_brightness = brightness;
        set_bit(&x52->update_mask, X52_BIT_BRI_LED);
    }

    return LIBX52_SUCCESS;
}

int libx52_set_shift(libx52_device *x52, uint8_t state)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (state) {
        set_bit(&x52->led_mask, X52_BIT_SHIFT);
    } else {
        clr_bit(&x52->led_mask, X52_BIT_SHIFT);
    }

    set_bit(&x52->update_mask, X52_BIT_SHIFT);
    return LIBX52_SUCCESS;
}

int libx52_set_blink(libx52_device *x52, uint8_t state)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (state) {
        set_bit(&x52->led_mask, X52_BIT_POV_BLINK);
    } else {
        clr_bit(&x52->led_mask, X52_BIT_POV_BLINK);
    }

    set_bit(&x52->update_mask, X52_BIT_POV_BLINK);
    return LIBX52_SUCCESS;
}
