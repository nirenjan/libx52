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
#include <stdbool.h>

#define PINELOG_MODULE X52D_MOD_DEVICE
#include "x52d_const.h"
#include "x52d_config.h"
#include "x52d_device.h"
#include "x52d_notify.h"
#include "libx52.h"
#include "pinelog.h"

static libx52_device *x52_dev;

static pthread_mutex_t device_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t device_thr;
static volatile bool device_update_needed;

static void *x52_dev_thr(void *param)
{
    int rc;

    #define DEV_ACQ_DELAY 5 // seconds
    #define DEV_UPD_DELAY 50000 // microseconds

    PINELOG_INFO(_("Starting X52 device manager thread"));
    for (;;) {
        if (!libx52_is_connected(x52_dev)) {
            PINELOG_TRACE("Attempting to connect to X52 device");
            rc = libx52_connect(x52_dev);
            if (rc != LIBX52_SUCCESS) {
                if (rc != LIBX52_ERROR_NO_DEVICE) {
                    PINELOG_ERROR(_("Error %d connecting to device: %s"),
                                  rc, libx52_strerror(rc));
                } else {
                    PINELOG_TRACE("No compatible X52 device found");
                }
                PINELOG_TRACE("Sleeping for %d seconds before trying to acquire device again", DEV_ACQ_DELAY);
                sleep(DEV_ACQ_DELAY);
            } else {
                /* Successfully connected */
                PINELOG_INFO(_("Device connected, writing configuration"));
                X52D_NOTIFY("CONNECTED");
                x52d_config_apply();
            }
        } else {
            if (!device_update_needed) {
                usleep(DEV_UPD_DELAY);
                continue;
            }

            (void)x52d_dev_update();
        }
    }

    #undef DEV_ACQ_DELAY
    #undef DEV_UPD_DELAY

    return NULL;
}

void x52d_dev_init(void)
{
    int rc;
    PINELOG_INFO(_("Initializing libx52"));
    rc = libx52_init(&x52_dev);

    if (rc != LIBX52_SUCCESS) {
        PINELOG_FATAL(_("Failure %d initializing libx52: %s"),
                      rc, libx52_strerror(rc));
    }

    // Create and initialize the thread
    pthread_create(&device_thr, NULL, x52_dev_thr, NULL);
}

void x52d_dev_exit(void)
{
    // Shutdown any threads
    PINELOG_INFO(_("Shutting down X52 device manager thread"));
    pthread_cancel(device_thr);

    libx52_exit(x52_dev);
}

#define WRAP_LIBX52(func) \
    int rc; \
    pthread_mutex_lock(&device_mutex); \
    rc = func; \
    pthread_mutex_unlock(&device_mutex); \
    if (rc != LIBX52_SUCCESS) { \
        if (rc != LIBX52_ERROR_TRY_AGAIN) { \
            PINELOG_ERROR(_("Error %d when updating X52 parameter: %s"), \
                          rc, libx52_strerror(rc)); \
        } \
    } else { \
        device_update_needed = true; \
    } \
    return rc

int x52d_dev_set_text(uint8_t line, const char *text, uint8_t length)
{
    WRAP_LIBX52(libx52_set_text(x52_dev, line, text, length));
}
int x52d_dev_set_led_state(libx52_led_id led, libx52_led_state state)
{
    if (libx52_check_feature(x52_dev, LIBX52_FEATURE_LED) != LIBX52_ERROR_NOT_SUPPORTED) {
        WRAP_LIBX52(libx52_set_led_state(x52_dev, led, state));
    }

    // If the target device does not support setting individual LEDs,
    // then ignore the set and let the caller think it succeeded.
    PINELOG_TRACE("Ignoring set LED state call as the device does not support it");
    return LIBX52_SUCCESS;
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
            // Detach from the existing device, the next thread run will
            // pick it up.
            PINELOG_TRACE("Disconnecting detached device");
            libx52_disconnect(x52_dev);
            X52D_NOTIFY("DISCONNECTED");
        } else {
            PINELOG_ERROR(_("Error %d when updating X52 device: %s"),
                          rc, libx52_strerror(rc));
        }
    } else {
        device_update_needed = false;
    }

    return rc;
}
