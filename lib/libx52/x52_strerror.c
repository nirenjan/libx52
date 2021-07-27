/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdio.h>

#include "libx52.h"
#include "gettext.h"

#define N_(str) gettext_noop(str)
#define _(str)  dgettext(PACKAGE, str)

/* Error buffer used for building custom error strings */
static char error_buffer[256];

/* List of error strings */
static const char *error_string[] = {
    N_("Success"),
    N_("Initialization failure"),
    N_("Insufficient memory"),
    N_("Invalid parameter"),
    N_("Operation not supported"),
    N_("Try again"),
    N_("Input parameter out of range"),
    N_("USB transaction failure"),
    N_("USB input/output error"),
    N_("Access denied"),
    N_("No such device"),
    N_("Entity not found"),
    N_("Resource busy"),
    N_("Operation timeout"),
    N_("Overflow"),
    N_("Pipe error"),
    N_("System call interrupted"),
};

const char * libx52_strerror(libx52_error_code error)
{
    switch (error) {
    case LIBX52_SUCCESS:
    case LIBX52_ERROR_INIT_FAILURE:
    case LIBX52_ERROR_OUT_OF_MEMORY:
    case LIBX52_ERROR_INVALID_PARAM:
    case LIBX52_ERROR_NOT_SUPPORTED:
    case LIBX52_ERROR_TRY_AGAIN:
    case LIBX52_ERROR_OUT_OF_RANGE:
    case LIBX52_ERROR_USB_FAILURE:
    case LIBX52_ERROR_IO:
    case LIBX52_ERROR_PERM:
    case LIBX52_ERROR_NO_DEVICE:
    case LIBX52_ERROR_NOT_FOUND:
    case LIBX52_ERROR_BUSY:
    case LIBX52_ERROR_TIMEOUT:
    case LIBX52_ERROR_OVERFLOW:
    case LIBX52_ERROR_PIPE:
    case LIBX52_ERROR_INTERRUPTED:
        return _(error_string[error]);

    default:
        snprintf(error_buffer, sizeof(error_buffer),
                _("Unknown error %d"), error);
        return error_buffer;
    }
}
