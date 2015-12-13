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

#include "libx52.h"
#include "x52_commands.h"
#include "x52_common.h"

int libx52_set_clock(libx52_device *x52, time_t time, int local)
{
    struct tm timeval;
    int local_tz;
    int local_date_day;
    int local_date_month;
    int local_date_year;
    int local_time_hour;
    int local_time_minute;
    int update_required = 0;

    if (!x52) {
        return -EINVAL;
    }

    if (local) {
        timeval = *localtime(&time);
        /* timezone from time.h presents the offset in seconds west of GMT.
         * Negate and divide by 60 to get the offset in minutes east of GMT.
         */
        local_tz = -timezone / 60;
    } else {
        timeval = *gmtime(&time);
        /* No offset from GMT */
        local_tz = 0;
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

        x52->date_day = local_date_day;
        x52->date_month = local_date_month;
        x52->date_year = local_date_year;
        set_bit(&x52->update_mask, X52_BIT_MFD_DATE);
        update_required = 1;
    }

    /* Update the time only if it has changed */
    if (x52->time_hour != local_time_hour ||
        x52->time_minute != local_time_minute) {

        x52->time_hour = local_time_hour;
        x52->time_minute = local_time_minute;
        set_bit(&x52->update_mask, X52_BIT_MFD_TIME);
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
    return (update_required ? 0 : -EAGAIN);
}

int libx52_set_clock_timezone(libx52_device *x52, libx52_clock_id clock, int offset)
{
    if (!x52) {
        return -EINVAL;
    }

    /* Limit offset to +/- 24 hours */
    if (offset < -1440 || offset > 1440) {
        return -EDOM;
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

    default:
        return -ENOTSUP;
    }

    return 0;
}

int libx52_set_clock_format(libx52_device *x52,
                            libx52_clock_id clock,
                            libx52_clock_format format)
{
    if (!x52) {
        return -EINVAL;
    }

    if ((format != LIBX52_CLOCK_FORMAT_12HR) &&
        (format != LIBX52_CLOCK_FORMAT_24HR)) {

       return -EINVAL;
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
        return -EINVAL;
    }

    x52->time_format[clock] = format;
    return 0;
}

int libx52_set_date_format(libx52_device *x52, libx52_date_format format)
{
    if (!x52) {
        return -EINVAL;
    }

    x52->date_format = format;
    set_bit(&x52->update_mask, X52_BIT_MFD_DATE);
    return 0;
}
