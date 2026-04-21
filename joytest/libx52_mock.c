/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stddef.h>
#include <time.h>
#include <stdint.h>

#include <libx52/libx52.h>

/* Opaque device: any unique non-NULL address is enough for joytest. */
static char joytest_mock_dev_storage;

int libx52_init(libx52_device **dev)
{
    if (!dev) {
        return LIBX52_ERROR_INVALID_PARAM;
    }
    *dev = (libx52_device *)&joytest_mock_dev_storage;
    return LIBX52_SUCCESS;
}

void libx52_exit(libx52_device *dev)
{
    (void)dev;
}

const char *libx52_strerror(libx52_error_code error)
{
    (void)error;
    // We don't actually use this in the mock, but it's required by the API
    return "";
}

int libx52_check_feature(libx52_device *x52, libx52_feature feature)
{
    (void)x52;
    (void)feature;
    // This is needed to satisfy the API, assume that the device supports LED control
    return LIBX52_SUCCESS;
}

int libx52_set_text(libx52_device *x52, uint8_t line, const char *text, uint8_t length)
{
    (void)x52;
    (void)line;
    (void)text;
    (void)length;
    return LIBX52_SUCCESS;
}

int libx52_set_led_state(libx52_device *x52, libx52_led_id led, libx52_led_state state)
{
    (void)x52;
    (void)led;
    (void)state;
    return LIBX52_SUCCESS;
}

int libx52_set_clock(libx52_device *x52, time_t t, int local)
{
    (void)x52;
    (void)t;
    (void)local;
    return LIBX52_SUCCESS;
}

int libx52_set_clock_timezone(libx52_device *x52, libx52_clock_id clock, int offset)
{
    (void)x52;
    (void)clock;
    (void)offset;
    return LIBX52_SUCCESS;
}

int libx52_set_clock_format(libx52_device *x52, libx52_clock_id clock, libx52_clock_format format)
{
    (void)x52;
    (void)clock;
    (void)format;
    return LIBX52_SUCCESS;
}

int libx52_set_date_format(libx52_device *x52, libx52_date_format format)
{
    (void)x52;
    (void)format;
    return LIBX52_SUCCESS;
}

int libx52_set_brightness(libx52_device *x52, uint8_t mfd, uint16_t brightness)
{
    (void)x52;
    (void)mfd;
    (void)brightness;
    return LIBX52_SUCCESS;
}

int libx52_set_shift(libx52_device *x52, uint8_t state)
{
    (void)x52;
    (void)state;
    return LIBX52_SUCCESS;
}

int libx52_set_blink(libx52_device *x52, uint8_t state)
{
    (void)x52;
    (void)state;
    return LIBX52_SUCCESS;
}

int libx52_update(libx52_device *x52)
{
    (void)x52;
    return LIBX52_SUCCESS;
}
