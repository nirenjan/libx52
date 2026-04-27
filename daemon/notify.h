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

/**
 * Emit legacy NUL notify plus framed IPC DEVICE_STATE.
 * @param connected Non-zero when the device is connected, zero on disconnect.
 * @param vid @c idVendor (meaningful when @p connected; on disconnect, pass last device ids).
 * @param pid @c idProduct
 * @param product_utf8 Optional product name (connect only); may be NULL when @p product_len is 0.
 */
void x52d_notify_device_state(int connected, uint16_t vid, uint16_t pid, const char *product_utf8,
    size_t product_len);

#define X52D_NOTIFY(...) do { \
    const char *argv ## __LINE__ [] = {__VA_ARGS__}; \
    x52d_notify_send(sizeof(argv ## __LINE__ )/sizeof(argv ## __LINE__ [0]), argv ## __LINE__ ); \
} while(0)

#endif // !defined X52D_NOTIFY_H

