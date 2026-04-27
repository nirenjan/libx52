/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file x52dcomm.h
 * @brief Functions, structures and enumerations for the Saitek X52 MFD & LED
 * daemon communication library.
 *
 * This file contains the type, enum and function prototypes for the Saitek X52
 * daemon communication library. These functions allow a client application to
 * communicate with a running X52 daemon, execute commands and retrieve data.
 *
 * @par Primary API: framed LIPC (\c x52d.socket)
 * Use @ref x52d_ipc_socket_path, @ref x52d_dial_ipc, and @ref x52d_ipc_call with
 * opcodes and field semantics from @ref x52d_ipc.h. This is the **only** supported
 * long-term path for daemon RPC and for **push** notifications on the same
 * socket (frames with \c lipc_header.tid == \c 0), including **DEVICE_STATE**
 * (USB connect/disconnect; see @ref proto_lipc_framed / @ref lipc_push_device_state
 * in the daemon protocol documentation).
 *
 * @par Deprecated: legacy NUL sockets
 * The helpers @ref x52d_dial_command, @ref x52d_format_command, @ref x52d_send_command,
 * @ref x52d_dial_notify, and @ref x52d_recv_notification target the historical
 * \c x52d.cmd and \c x52d.notify NUL-terminated protocols. They remain for
 * migration only, are **deprecated**, and **will be removed** in a future release
 * when those sockets are dropped. New code must use the framed IPC entry points
 * above.
 *
 * @author Nirenjan Krishnan (nirenjan@nirenjan.org)
 */
#ifndef X52DCOMM_H
#define X52DCOMM_H

#include <stddef.h>
#include <stdint.h>

#include <libx52/x52d_ipc.h>
#include <localipc/lipc.h>

#ifndef X52DCOMM_API
# if defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
#  define X52DCOMM_API __attribute__((visibility("default")))
# elif defined(_WIN32)
#  define X52DCOMM_API __declspec(dllexport)
# else
#  define X52DCOMM_API
# endif
#endif

/**
 * Define @c X52DCOMM_NO_DEPRECATED_ATTR before including this header to compile
 * without @c deprecated attributes on legacy entry points (for in-tree sources
 * that still call them until migration completes).
 */
#if defined(X52DCOMM_NO_DEPRECATED_ATTR)
# define X52DCOMM_DEPRECATED
#elif defined(__GNUC__) || defined(__clang__)
# define X52DCOMM_DEPRECATED __attribute__((deprecated("use x52d_ipc_socket_path, x52d_dial_ipc, and x52d_ipc_call")))
#elif defined(_MSC_VER)
# define X52DCOMM_DEPRECATED __declspec(deprecated("use x52d_ipc_socket_path, x52d_dial_ipc, and x52d_ipc_call"))
#else
# define X52DCOMM_DEPRECATED
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup x52dcomm Daemon communication
 *
 * These functions are used to communicate with the X52 daemon.
 *
 * @{
 */

/**
 * @brief Resolve the framed IPC UNIX socket path used by x52d.
 *
 * If @p sock_path is NULL, returns the default path compiled into the library
 * (same layout as the running daemon’s @c RUNDIR socket).
 *
 * @param[in] sock_path Optional override path, or NULL for the default.
 * @return Pointer to a NUL-terminated path string (must not be freed).
 */
X52DCOMM_API const char *x52d_ipc_socket_path(const char *sock_path);

/**
 * @brief Open a blocking connection to the daemon framed IPC socket.
 *
 * Use @ref x52d_ipc_call to issue requests. Close the descriptor with @c close(2)
 * when finished.
 *
 * @param[in] sock_path Optional path override, or NULL for the default path from @ref x52d_ipc_socket_path.
 * @returns Connected socket fd on success, or @c -1 with @c errno set.
 */
X52DCOMM_API int x52d_dial_ipc(const char *sock_path);

/**
 * @brief Perform one synchronous framed IPC RPC on a connected fd.
 *
 * Allocates a short-lived liblocalipc client, sends one request, blocks until the
 * matching reply (dispatching unrelated @c tid==0 pushes while waiting), then returns.
 * Unrelated notifications are accepted and ignored unless liblocalipc is extended
 * later with explicit dispatch hooks.
 *
 * @param[in] fd Connected stream socket from @ref x52d_dial_ipc.
 * @param[in] request_id Wire @c request opcode (e.g. @c X52D_IPC_CONFIG_DUMP).
 * @param[in] index @c lipc_header.index (opcode-specific).
 * @param[in] value @c lipc_header.value (opcode-specific).
 * @param[in] payload Request payload, or NULL when @p payload_len is 0.
 * @param[in] payload_len Request payload length in bytes.
 * @param[out] reply_hdr Decoded reply header, or NULL if not needed.
 * @param[out] reply_payload Buffer for reply payload, or NULL when @p reply_payload_cap is 0.
 * @param[in] reply_payload_cap Capacity of @p reply_payload.
 * @param[out] reply_len Stored reply payload length; may be NULL.
 *
 * @return \c LIPC_OK when a reply was received and captured; other \c lipc_status
 *         values on protocol or I/O failure (\c errno may apply for \c LIPC_IO_ERROR).
 */
X52DCOMM_API lipc_status x52d_ipc_call(int fd, uint16_t request_id, uint16_t index, uint64_t value,
    const void *payload, size_t payload_len,
    lipc_header *reply_hdr, void *reply_payload, size_t reply_payload_cap, size_t *reply_len);

/**
 * @brief Decode a DEVICE_STATE server push (@c X52D_IPC_PUSH_DEVICE_STATE, @c tid == 0).
 *
 * On success, @p connected is 1 when @c lipc_header.index indicates connected, else 0.
 * @p name_utf8 and @p name_len describe the optional UTF-8 product substring in @p payload
 * (not necessarily NUL-terminated; use @p name_len). Either name output may be NULL.
 *
 * @return 0 on success, -1 when @p hdr is not a well-formed DEVICE_STATE push.
 */
X52DCOMM_API int x52d_ipc_device_state_decode(const lipc_header *hdr, const void *payload,
    size_t payload_len, int *connected, uint16_t *vid, uint16_t *pid, const char **name_utf8,
    size_t *name_len);

/**
 * @brief Open a connection to the daemon command socket.
 *
 * @deprecated Legacy NUL-separated command socket; use @ref x52d_dial_ipc and
 *            @ref x52d_ipc_call instead. This entry point will be removed when
 *            legacy sockets are dropped.
 */
X52DCOMM_API X52DCOMM_DEPRECATED int x52d_dial_command(const char *sock_path);

/**
 * @brief Open a connection to the daemon notify socket.
 *
 * @deprecated Legacy NUL notify socket (\c x52d.notify). Subscribe on the unified
 *            framed IPC socket (\ref x52d_dial_ipc) and handle \c tid==0 pushes
 *            (e.g. \c DEVICE_STATE; see @ref proto_lipc_framed). Removed in a future
 *            release with the legacy socket.
 */
X52DCOMM_API X52DCOMM_DEPRECATED int x52d_dial_notify(const char *sock_path);

/**
 * @brief Format a series of command strings into a buffer
 *
 * @deprecated Legacy command encoding for @ref x52d_send_command; use @ref x52d_ipc_call.
 */
X52DCOMM_API X52DCOMM_DEPRECATED int x52d_format_command(int argc, const char **argv, char *buffer, size_t buflen);

/**
 * @brief Send a command to the daemon and retrieve the response.
 *
 * @deprecated Legacy NUL-separated command protocol; use @ref x52d_ipc_call.
 */
X52DCOMM_API X52DCOMM_DEPRECATED int x52d_send_command(int sock_fd, char *buffer, size_t bufin, size_t bufout);

/**
 * @brief Notification callback function type
 */
typedef int (* x52d_notify_callback_fn)(int argc, char **argv);

/**
 * @brief Receive a notification from the daemon
 *
 * @deprecated Legacy NUL notify protocol; use framed IPC (\c tid==0) pushes on the
 *            same socket as @ref x52d_dial_ipc.
 */
X52DCOMM_API X52DCOMM_DEPRECATED int x52d_recv_notification(int sock_fd, x52d_notify_callback_fn callback);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* X52DCOMM_H */
