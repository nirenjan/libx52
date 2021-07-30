/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#define _GNU_SOURCE
#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "libx52.h"
#include "x52_commands.h"
#include "x52_common.h"

#if !HAVE_STRUCT_TM_TM_GMTOFF
/* Slow method to compute GMT offset */
static int _x52_compute_gmtoff(struct tm *timeval)
{
    char buf[20];
    size_t written;
    int offset;

    /* This function uses strftime to get the timezone as a string, then
     * converts that back into an integer offset.
     */
    written = strftime(buf, sizeof(buf), "%z", timeval);
    if (written != 5) {
        /* Expect %z to return 5 characters - e.g. +0530, -0700, +0000 */
        return 0;
    }

    offset = 600 * (buf[1] - '0') + 60 * (buf[2] - '0') + 10 * (buf[3] - '0') + (buf[4] - '0');
    if (buf[0] == '-') {
        offset = -offset;
    } else if (buf[0] != '+') {
        offset = 0;
    }
    return offset;
}
#endif

int libx52_set_clock(libx52_device *x52, time_t time, int local)
{
    struct tm timeval;
    struct tm *ptr;
    int local_tz;
    int local_date_day;
    int local_date_month;
    int local_date_year;
    int local_time_hour;
    int local_time_minute;
    int update_required = 0;

    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if (local) {
        ptr = localtime_r(&time, &timeval);
        #if HAVE_STRUCT_TM_TM_GMTOFF
        /* If valid, then timeval.tm_gmtoff contains the offset in seconds
         * east of GMT. Divide by 60 to get the offset in minutes east of GMT.
         */
        local_tz = (int)(timeval.tm_gmtoff / 60);
        #else
        local_tz = _x52_compute_gmtoff(&timeval);
        #endif
    } else {
        ptr = gmtime_r(&time, &timeval);
        /* No offset from GMT */
        local_tz = 0;
    }

    if (ptr == NULL) {
        /* Time conversion failed. This happens if the time value exceeds the
         * range which can be represented.
         */
        return LIBX52_ERROR_OUT_OF_RANGE;
    }

    local_date_day = timeval.tm_mday;
    local_date_month = timeval.tm_mon + 1;
    local_date_year = timeval.tm_year % 100;
    local_time_hour = timeval.tm_hour;
    local_time_minute = timeval.tm_min;

    /* Update the date only if it has changed */
    if (x52->date_day != local_date_day ||
        x52->date_month != local_date_month ||
        x52->date_year != local_date_year) {

        libx52_set_date(x52, local_date_day, local_date_month, local_date_year);
        update_required = 1;
    }

    /* Update the time only if it has changed */
    if (x52->time_hour != local_time_hour ||
        x52->time_minute != local_time_minute) {

        libx52_set_time(x52, local_time_hour, local_time_minute);
        update_required = 1;
    }

    /* Update the offset fields only if the timezone has changed */
    if (x52->timezone[LIBX52_CLOCK_1] != local_tz) {
        set_bit(&x52->update_mask, X52_BIT_MFD_OFFS1);
        set_bit(&x52->update_mask, X52_BIT_MFD_OFFS2);
        update_required = 1;
    }

    /* Save the timezone */
    x52->timezone[LIBX52_CLOCK_1] = local_tz;
    return (update_required ? LIBX52_SUCCESS : LIBX52_ERROR_TRY_AGAIN);
}

int libx52_set_time(libx52_device *x52, uint8_t hour, uint8_t minute)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    x52->time_hour = hour;
    x52->time_minute = minute;
    set_bit(&x52->update_mask, X52_BIT_MFD_TIME);

    return LIBX52_SUCCESS;
}

int libx52_set_date(libx52_device *x52, uint8_t dd, uint8_t mm, uint8_t yy)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    x52->date_day = dd;
    x52->date_month = mm;
    x52->date_year = yy;
    set_bit(&x52->update_mask, X52_BIT_MFD_DATE);

    return LIBX52_SUCCESS;
}

int libx52_set_clock_timezone(libx52_device *x52, libx52_clock_id clock, int offset)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Limit offset to +/- 24 hours */
    if (offset < -1440 || offset > 1440) {
        return LIBX52_ERROR_OUT_OF_RANGE;
    }

    switch (clock) {
    case LIBX52_CLOCK_2:
        x52->timezone[clock] = offset;
        set_bit(&x52->update_mask, X52_BIT_MFD_OFFS1);
        break;

    case LIBX52_CLOCK_3:
        x52->timezone[clock] = offset;
        set_bit(&x52->update_mask, X52_BIT_MFD_OFFS2);
        break;

    case LIBX52_CLOCK_1:
        return LIBX52_ERROR_NOT_SUPPORTED;

    default:
        return LIBX52_ERROR_INVALID_PARAM;
    }

    return LIBX52_SUCCESS;
}

int libx52_set_clock_format(libx52_device *x52,
                            libx52_clock_id clock,
                            libx52_clock_format format)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    if ((format != LIBX52_CLOCK_FORMAT_12HR) &&
        (format != LIBX52_CLOCK_FORMAT_24HR)) {

       return LIBX52_ERROR_INVALID_PARAM;
    }

    switch (clock) {
    case LIBX52_CLOCK_1:
        set_bit(&x52->update_mask, X52_BIT_MFD_TIME);
        break;

    case LIBX52_CLOCK_2:
        set_bit(&x52->update_mask, X52_BIT_MFD_OFFS1);
        break;

    case LIBX52_CLOCK_3:
        set_bit(&x52->update_mask, X52_BIT_MFD_OFFS2);
        break;

    default:
        return LIBX52_ERROR_INVALID_PARAM;
    }

    x52->time_format[clock] = format;
    return LIBX52_SUCCESS;
}

int libx52_set_date_format(libx52_device *x52, libx52_date_format format)
{
    if (!x52) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    x52->date_format = format;
    set_bit(&x52->update_mask, X52_BIT_MFD_DATE);
    return LIBX52_SUCCESS;
}
