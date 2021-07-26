/*
 * Saitek X52 Pro MFD & LED driver - Clock manager
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_clock.h"
#include "x52d_const.h"
#include "x52d_device.h"

static bool clock_enabled = false;
static int clock_primary_is_local = false;

void x52d_cfg_set_Clock_Enabled(bool enabled)
{
    PINELOG_DEBUG(_("Setting clock enable to %s"),
                  enabled ? "on" : "off");
    clock_enabled = enabled;
}

void x52d_cfg_set_Clock_PrimaryIsLocal(bool param)
{
    PINELOG_DEBUG(_("Setting primary clock timezone to %s"),
                  param ? "local" : "UTC");
    clock_primary_is_local = !!param;
}

static int get_tz_offset(const char *tz)
{
    char *orig_tz = NULL;
    char *orig_tz_copy = NULL;
    time_t t;
    struct tm *timeval;
    char *new_tz = NULL;
    size_t new_tz_len;
    int offset = 0;

    new_tz_len = strlen(tz) + 2;
    new_tz = malloc(new_tz_len);
    if (new_tz == NULL) {
        PINELOG_WARN(_("Unable to allocate memory for timezone. Falling back to UTC"));
        goto cleanup;
    }
    snprintf(new_tz, new_tz_len, ":%s", tz);

    orig_tz = getenv("TZ");
    if (orig_tz != NULL) {
        /* TZ was set in the environment */
        orig_tz_copy = strdup(orig_tz);
        if (orig_tz_copy == NULL) {
            PINELOG_WARN(_("Unable to backup timezone environment. Falling back to UTC"));
            goto cleanup;
        }
    }

    setenv("TZ", new_tz, true);
    t = time(NULL);
    timeval = localtime(&t);
    if (timeval != NULL) {
        #if HAVE_STRUCT_TM_TM_GMTOFF
        /* If valid, then timeval.tm_gmtoff contains the offset in seconds east
         * of GMT. Divide by 60 to get the offset in minutes east of GMT.
         */
        offset = (int)(timeval->tm_gmtoff / 60);
        #else
        /* The compiler does not provide tm_gmtoff. Fallback to using the
         * timezone variable, which is in seconds west of GMT. Divide by -60 to
         * get the offset in minutes east of GMT.
         *
         * ============
         * XXX NOTE XXX
         * ============
         * timezone is always the default (non-summer) timezone offset from GMT.
         * Therefore, this may not be accurate during the summer time months
         * for the region in question.
         */
        offset = (int)(timezone / -60);
        #endif
    }

cleanup:
    if (orig_tz == NULL) {
        unsetenv("TZ");
    } else {
        setenv("TZ", orig_tz_copy, true);
        free(orig_tz_copy);
    }

    if (new_tz != NULL) {
        free(new_tz);
    }

    tzset();
    PINELOG_DEBUG("Offset for timezone '%s' is %d", tz, offset);
    return offset;
}

void x52d_cfg_set_Clock_Secondary(char* param)
{
    PINELOG_TRACE("Setting secondary clock timezone to %s", param);
    x52d_dev_set_clock_timezone(LIBX52_CLOCK_2, get_tz_offset(param));
}

void x52d_cfg_set_Clock_Tertiary(char* param)
{
    PINELOG_TRACE("Setting tertiary clock timezone to %s", param);
    x52d_dev_set_clock_timezone(LIBX52_CLOCK_3, get_tz_offset(param));
}

static void set_clock_format(const char *name, libx52_clock_id id, libx52_clock_format fmt)
{
    PINELOG_TRACE("Setting %s clock format to %s", name,
                  fmt == LIBX52_CLOCK_FORMAT_12HR ? "12 hour" : "24 hour");
    x52d_dev_set_clock_format(id, fmt);
}

void x52d_cfg_set_Clock_FormatPrimary(libx52_clock_format fmt)
{
    set_clock_format("primary", LIBX52_CLOCK_1, fmt);
}

void x52d_cfg_set_Clock_FormatSecondary(libx52_clock_format fmt)
{
    set_clock_format("secondary", LIBX52_CLOCK_2, fmt);
}

void x52d_cfg_set_Clock_FormatTertiary(libx52_clock_format fmt)
{
    set_clock_format("tertiary", LIBX52_CLOCK_3, fmt);
}

void x52d_cfg_set_Clock_DateFormat(libx52_date_format fmt)
{
    static const char *formats[] = {
        "dd-mm-yy",
        "mm-dd-yy",
        "yy-mm-dd"
    };
    PINELOG_TRACE("Setting date format to %s", formats[fmt]);
    x52d_dev_set_date_format(fmt);
}

static pthread_t clock_thr;

static void * x52_clock_thr(void *param)
{
    int rc;

    PINELOG_INFO(_("Starting X52 clock manager thread"));
    for (;;) {
        time_t cur_time;

        sleep(1);
        if (!clock_enabled) {
            /* Clock thread is disabled, check again next time */
            continue;
        }

        if (time(&cur_time) < 0) {
            PINELOG_WARN(_("Error %d retrieving current time: %s"),
                         errno, strerror(errno));
            continue;
        }
        rc = x52d_dev_set_clock(cur_time, clock_primary_is_local);
        if (rc == LIBX52_SUCCESS) {
            // Device manager will update the clock, this is only for debugging
            PINELOG_TRACE("Setting X52 clock to %ld", cur_time);
        }
    }

    return NULL;
}

void x52d_clock_init(void)
{
    int rc;

    PINELOG_TRACE("Initializing clock manager");
    rc = pthread_create(&clock_thr, NULL, x52_clock_thr, NULL);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d initializing clock thread: %s"),
                      rc, strerror(rc));
    }
}

void x52d_clock_exit(void)
{
    PINELOG_INFO(_("Shutting down X52 clock manager thread"));
    pthread_cancel(clock_thr);
}
