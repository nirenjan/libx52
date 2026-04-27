/*
 * Saitek X52 Pro MFD & LED driver - framed IPC listener (unified command + notify)
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_IPC_SERVICE_H
#define X52D_IPC_SERVICE_H

/**
 * Start the framed-IPC server on the given path (NULL = default from constants.h).
 * One UNIX socket serves both RPC-style requests and server-initiated pushes.
 *
 * @return 0 on success, -1 on failure (errno set where applicable).
 */
int x52d_ipc_init(const char *sock_path);

/** Stop the listener (wake + join + close) and remove the bound socket path. */
void x52d_ipc_exit(void);

/**
 * Broadcast DEVICE_STATE (\c X52D_IPC_PUSH_DEVICE_STATE) to all framed-IPC clients.
 * No-op when the IPC server is not running. @p index: 0 = disconnected, 1 = connected;
 * @p value lower 32 bits encode \c (vid<<16)|pid per @ref x52d_ipc_device_state_pack_usb.
 */
void x52d_ipc_push_device_state(uint16_t index, uint64_t value, const void *payload,
    size_t payload_len);

#endif /* !defined(X52D_IPC_SERVICE_H) */
