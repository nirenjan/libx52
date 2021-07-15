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

#include "gettext.h"
#define _(x) gettext(x)

#endif // !defined X52D_CONST_H
