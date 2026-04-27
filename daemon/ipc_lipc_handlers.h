/*
 * Saitek X52 Pro MFD & LED driver - framed IPC opcode handlers
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_IPC_LIPC_HANDLERS_H
#define X52D_IPC_LIPC_HANDLERS_H

#include <stddef.h>
#include <libx52/x52d_ipc.h>
#include <localipc/lipc.h>

const lipc_server_handler *x52d_ipc_handlers_table(void);
size_t x52d_ipc_handlers_count(void);

#endif /* !defined X52D_IPC_LIPC_HANDLERS_H */
