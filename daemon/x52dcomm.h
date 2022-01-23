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
 * @author Nirenjan Krishnan (nirenjan@nirenjan.org)
 */
#ifndef X52DCOMM_H
#define X52DCOMM_H

#include <stddef.h>

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
 * @brief Open a connection to the daemon command socket.
 *
 * This method opens a socket connection to the daemon command socket. This
 * socket allows the client to issue commands and retrieve data. The \p sock_path
 * parameter may be NULL, in which case, it will use the default socket path.
 *
 * The client will need to use the returned descriptor to communicate with the
 * daemon using \ref x52d_send_command. Once finished, the client may use the
 * \c close(2) method to close the file descriptor.
 *
 * @param[in]   sock_path   Path to the daemon command socket.
 *
 * @returns Non-negative socket file descriptor on success.
 * @returns -1 on failure, and set \c errno accordingly.
 *
 * @exception E2BIG returned if the passed socket path is too big
 */
int x52d_dial_command(const char *sock_path);

/**
 * @brief Open a connection to the daemon notify socket.
 *
 * This method opens a socket connection to the daemon notify socket. This
 * socket allows the client to receive notifications from the daemon. Thej
 * \p sock_path parameter may be NULL, in which case, it will use the default
 * socket path.
 *
 * The client will need to use the returned descriptor to communicate with the
 * daemon using \ref x52d_recv_notification. Once finished, the client may use
 * the \c close(2) method to close the file descriptor.
 *
 * @param[in]   sock_path   Path to the daemon command socket.
 *
 * @returns Non-negative socket file descriptor on success.
 * @returns -1 on failure, and set \c errno accordingly.
 *
 * @exception E2BIG returned if the passed socket path is too big
 */
int x52d_dial_notify(const char *sock_path);

/**
 * @brief Format a series of command strings into a buffer
 *
 * The client sends the command and parameters as a series of NUL terminated
 * strings. This function concatenates the commands into a single buffer that
 * can be passed to \ref x52d_send_command.
 *
 * \p buffer should be at least 1024 bytes long.
 *
 * @param[in]   argc    Number of arguments to fit in the buffer
 * @param[in]   argv    Pointer to an array of arguments.
 * @param[out]  buffer  Buffer to store the formatted command
 * @param[in]   buflen  Length of the buffer
 *
 * @returns number of bytes in the formatted command
 * @returns -1 on an error condition, and \c errno is set accordingly.
 */
int x52d_format_command(int argc, const char **argv, char *buffer, size_t buflen);

/**
 * @brief Send a command to the daemon and retrieve the response.
 *
 * The client sends the command and parameters as a series of NUL terminated
 * strings, and retrieves the response in the same manner. Depending on the
 * result, the return status is either a positive integer or -1, and \c errno
 * is set accordingly.
 *
 * \p buffer should contain sufficient space to accomodate the returned
 * response string.
 *
 * This is a blocking function and will not return until either a response is
 * received from the server, or an exception condition occurs.
 *
 * @param[in]       sock_fd Socket descriptor returned from
 *                          \ref x52d_dial_command
 *
 * @param[inout]    buffer  Pointer to the string containing the command and
 *                          parameters. This is also used to save the returned
 *                          response.
 *
 * @param[in]       bufin   Length of the command in the input buffer
 * @param[in]       bufout  Maximum length of the response
 *
 * @returns number of bytes returned from the server
 * @returns -1 on an error condition, and \c errno is set accordingly.
 */
int x52d_send_command(int sock_fd, char *buffer, size_t bufin, size_t bufout);

/**
 * @brief Notification callback function type
 */
typedef int (* x52d_notify_callback_fn)(int argc, char **argv);

/**
 * @brief Receive a notification from the daemon
 *
 * This function blocks until it receives a notification from the daemon. Once
 * it receives a notification successfully, it will call the callback function
 * with the arguments as string pointers. It will return the return value of
 * the callback function, if it was called.
 *
 * This is a blocking function and will not return until either a notification
 * is received from the server, or an exception condition occurs.
 *
 * @param[in]       sock_fd     Socket descriptor returned from
 *                              \ref x52d_dial_notify
 *
 * @param[in]       callback    Pointer to the callback function
 *
 * @returns return code of the callback function on success
 * @returns -1 on an error condition, and \c errno is set accordingly.
 */
int x52d_recv_notification(int sock_fd, x52d_notify_callback_fn callback);

/** @} */
#ifdef __cplusplus
}
#endif
#endif // !defined X52DCOMM_H


