/*
 * Saitek X52 Pro MFD & LED driver - Device manager
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <unistd.h>
#include <pthread.h>

#include "x52d_const.h"
#include "x52d_device.h"
#include "libx52.h"
#include "pinelog.h"

/*
 * Functions
 *
 * Thread method to scan and reconnect to the joystick if it has been
 * disconnected/never connected.
 *
 * API wrappers to libx52 functions
 */

static libx52_device *x52_dev;

static pthread_mutex_t device_mutex = PTHREAD_MUTEX_INITIALIZER;

#if 0
static void *x52_dev_thread(void *param)
{
    int rc;

    // Check if the device is connected in a loop
    for (;;) {
        #define RECONNECT_DELAY 5
        sleep(RECONNECT_DELAY);
        if (!libx52_is_connected(x52_dev)) {
            PINELOG_TRACE("Attempting to connect to X52 device");
            rc = libx52_connect(x52_dev);
            if (rc != LIBX52_SUCCESS) {
                if (rc != LIBX52_ERROR_NO_DEVICE) {
                    PINELOG_ERROR(_("Error %d connecting to device: %s"),
                                  rc, libx52_strerror(rc));
                }
            }
        }
    }

    return NULL;
}
#endif

void x52d_dev_init(void)
{
    int rc;
    PINELOG_TRACE("Initializing libx52");
    rc = libx52_init(&x52_dev);

    if (rc != LIBX52_SUCCESS) {
        PINELOG_FATAL(_("Failure %d initializing libx52: %s"),
                      rc, libx52_strerror(rc));
    }

    // TODO: Create and initialize the threads
}

void x52d_dev_exit(void)
{
    // TODO: Shutdown any threads

    libx52_exit(x52_dev);
}

#define WRAP_LIBX52(func) \
    int rc; \
    pthread_mutex_lock(&device_mutex); \
    rc = func; \
    pthread_mutex_unlock(&device_mutex); \
    if (rc != LIBX52_SUCCESS) { \
        PINELOG_ERROR(_("Error %d when updating X52 parameter: %s"), \
                      rc, libx52_strerror(rc)); \
    } \
    return rc

int x52d_dev_set_text(uint8_t line, const char *text, uint8_t length)
{
    WRAP_LIBX52(libx52_set_text(x52_dev, line, text, length));
}
int x52d_dev_set_led_state(libx52_led_id led, libx52_led_state state)
{
    WRAP_LIBX52(libx52_set_led_state(x52_dev, led, state));
}
int x52d_dev_set_clock(time_t time, int local)
{
    WRAP_LIBX52(libx52_set_clock(x52_dev, time, local));
}
int x52d_dev_set_clock_timezone(libx52_clock_id clock, int offset)
{
    WRAP_LIBX52(libx52_set_clock_timezone(x52_dev, clock, offset));
}
int x52d_dev_set_clock_format(libx52_clock_id clock, libx52_clock_format format)
{
    WRAP_LIBX52(libx52_set_clock_format(x52_dev, clock, format));
}
int x52d_dev_set_time(uint8_t hour, uint8_t minute)
{
    WRAP_LIBX52(libx52_set_time(x52_dev, hour, minute));
}
int x52d_dev_set_date(uint8_t dd, uint8_t mm, uint8_t yy)
{
    WRAP_LIBX52(libx52_set_date(x52_dev, dd, mm, yy));
}
int x52d_dev_set_date_format(libx52_date_format format)
{
    WRAP_LIBX52(libx52_set_date_format(x52_dev, format));
}
int x52d_dev_set_brightness(uint8_t mfd, uint16_t brightness)
{
    WRAP_LIBX52(libx52_set_brightness(x52_dev, mfd, brightness));
}
int x52d_dev_set_shift(uint8_t state)
{
    WRAP_LIBX52(libx52_set_shift(x52_dev, state));
}
int x52d_dev_set_blink(uint8_t state)
{
    WRAP_LIBX52(libx52_set_blink(x52_dev, state));
}
int x52d_dev_update(void)
{
    int rc;

    pthread_mutex_lock(&device_mutex);
    rc = libx52_update(x52_dev);
    pthread_mutex_unlock(&device_mutex);

    if (rc != LIBX52_SUCCESS) {
        if (rc == LIBX52_ERROR_NO_DEVICE) {
            // TODO: Detach and spawn thread to reconnect
            libx52_disconnect(x52_dev);
        } else {
            PINELOG_ERROR(_("Error %d when updating X52 device: %s"),
                          rc, libx52_strerror(rc));
        }
    }

    return rc;
}
