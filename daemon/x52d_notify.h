/*
 * Saitek X52 Pro MFD & LED driver - Notification manager
 *
 * Copyright (C) 2022 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_NOTIFY_H
#define X52D_NOTIFY_H

void x52d_notify_init(const char *notify_sock_path);
void x52d_notify_exit(void);
void x52d_notify_send(int argc, const char **argv);

#endif // !defined X52D_NOTIFY_H

