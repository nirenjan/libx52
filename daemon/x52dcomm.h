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
 * @brief Open a connection to the daemon.
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
 * @brief Send a command to the daemon and retrieve the response.
 *
 * The client sends the command and parameters as a single NULL terminated
 * string, and retrieves the response in the same manner. Depending on the
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

/** @} */
#ifdef __cplusplus
}
#endif
#endif // !defined X52DCOMM_H


