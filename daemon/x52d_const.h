/*
 * Saitek X52 Pro MFD & LED driver - Application constants
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_CONST_H
#define X52D_CONST_H

#define X52D_APP_NAME   "x52d"

#define X52D_LOG_FILE   LOGDIR "/" X52D_APP_NAME ".log"

#define X52D_SYS_CFG_FILE   SYSCONFDIR "/" X52D_APP_NAME "/" X52D_APP_NAME ".conf"

#define X52D_PID_FILE   RUNDIR "/" X52D_APP_NAME ".pid"

#define X52D_SOCK_COMMAND   RUNDIR "/" X52D_APP_NAME ".cmd"
#define X52D_SOCK_NOTIFY    RUNDIR "/" X52D_APP_NAME ".notify"

#include "gettext.h"
#define N_(x) gettext_noop(x)
#define _(x) gettext(x)

#define X52D_MAX_CLIENTS    63

enum {
    X52D_MOD_CONFIG,
    X52D_MOD_CLOCK,
    X52D_MOD_DEVICE,
    X52D_MOD_IO,
    X52D_MOD_LED,
    X52D_MOD_MOUSE,
    X52D_MOD_KEYBOARD,
    X52D_MOD_PROFILE,
    X52D_MOD_CLUTCH,
    X52D_MOD_COMMAND,
    X52D_MOD_CLIENT,
    X52D_MOD_NOTIFY,

    X52D_MOD_MAX
};

#endif // !defined X52D_CONST_H
