/*
 * Saitek X52 Pro MFD & LED driver - Application constants
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_CONST_H
#define X52D_CONST_H

#include <libx52/x52d_ipc.h>

#define X52D_APP_NAME   "x52d"

#define X52D_LOG_FILE   LOGDIR "/" X52D_APP_NAME ".log"

#define X52D_SYS_CFG_FILE   SYSCONFDIR "/" X52D_APP_NAME "/" X52D_APP_NAME ".conf"

/** Persistent runtime configuration (not under ephemeral @c RUNDIR). */
#define X52D_STATE_CFG_FILE   LOCALSTATEDIR "/lib/" X52D_APP_NAME "/" X52D_APP_NAME ".conf"

/** @deprecated Use @ref X52D_IPC_CONFIG_CLEAR_TARGET_STATE in @c libx52/x52d_ipc.h. */
#define X52D_CONFIG_CLEAR_TARGET_STATE   X52D_IPC_CONFIG_CLEAR_TARGET_STATE
/** @deprecated Use @ref X52D_IPC_CONFIG_CLEAR_TARGET_SYSCONF in @c libx52/x52d_ipc.h. */
#define X52D_CONFIG_CLEAR_TARGET_SYSCONF X52D_IPC_CONFIG_CLEAR_TARGET_SYSCONF

#define X52D_PID_FILE   RUNDIR "/" X52D_APP_NAME ".pid"

#define X52D_SOCK_COMMAND   RUNDIR "/" X52D_APP_NAME ".cmd"
#define X52D_SOCK_NOTIFY    RUNDIR "/" X52D_APP_NAME ".notify"

/** Framed IPC socket: RPC commands and async pushes share one path (legacy NUL \c .cmd / \c .notify remain until removal). */
#define X52D_SOCK_IPC   RUNDIR "/" X52D_APP_NAME ".socket"

#include "gettext.h"
#define N_(x) gettext_noop(x)
#define _(x) gettext(x)

#define X52D_MAX_CLIENTS    63

#include "module-map.h" // For module IDs

#endif // !defined X52D_CONST_H
