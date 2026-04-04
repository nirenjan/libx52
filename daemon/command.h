/*
 * Saitek X52 Pro MFD & LED driver - Command processor
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_COMMAND_H
#define X52D_COMMAND_H

int x52d_command_init(const char *sock_path);
void x52d_command_exit(void);
int x52d_command_loop(int sock_fd);

#endif // !defined X52D_COMMAND_H
