/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#include <stdio.h>

#include "libx52.h"

/** For future use in i18n */
#define _(str)  str

/* Error buffer used for building custom error strings */
static char error_buffer[256];

char * libx52_strerror(libx52_error_code error)
{
    switch (error) {
    case LIBX52_SUCCESS:
        return _("Success");

    case LIBX52_ERROR_INIT_FAILURE:
        return _("Initialization failure");

    case LIBX52_ERROR_OUT_OF_MEMORY:
        return _("Insufficient memory");

    case LIBX52_ERROR_INVALID_PARAM:
        return _("Invalid parameter");

    case LIBX52_ERROR_NOT_SUPPORTED:
        return _("Operation not supported");

    case LIBX52_ERROR_TRY_AGAIN:
        return _("Try again");

    case LIBX52_ERROR_OUT_OF_RANGE:
        return _("Input parameter out of range");

    case LIBX52_ERROR_USB_FAILURE:
        return _("USB transaction failure");

    case LIBX52_ERROR_IO:
        return _("USB input/output error");

    case LIBX52_ERROR_PERM:
        return _("Access denied");

    case LIBX52_ERROR_NO_DEVICE:
        return _("No such device");

    case LIBX52_ERROR_NOT_FOUND:
        return _("Entity not found");

    case LIBX52_ERROR_BUSY:
        return _("Resource busy");

    case LIBX52_ERROR_TIMEOUT:
        return _("Operation timeout");

    case LIBX52_ERROR_OVERFLOW:
        return _("Overflow");

    case LIBX52_ERROR_PIPE:
        return _("Pipe error");

    case LIBX52_ERROR_INTERRUPTED:
        return _("System call interrupted");

    default:
        snprintf(error_buffer, sizeof(error_buffer),
                _("Unknown error %d"), error);
        return error_buffer;
    }
}
