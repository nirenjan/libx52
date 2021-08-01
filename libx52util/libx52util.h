/*
 * Saitek X52 Pro Utility Library
 *
 * Copyright (C) 2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file libx52util.h
 * @brief Utility functions for the Saitek X52 MFD & LED driver library
 *
 * This file contains the type and function prototypes for the Saitek X52
 * driver library utility functions. These functions simplify some of the
 * data handling for \ref libx52mfdled, but they are not required.
 *
 * @author Nirenjan Krishnan (nirenjan@nirenjan.org)
 */
#ifndef LIBX52UTIL_H
#define LIBX52UTIL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup libx52util Utility Functions
 *
 * These functions simplify some of the data handling for \ref libx52mfdled.
 *
 * @{
 */

/**
 * @brief Convert UTF8 string to X52 character map.
 *
 * This function takes in a UTF-8 string and converts it to the character
 * map used by the X52Pro MFD. Unrecognized characters are silently dropped.
 *
 * @param[in]       input   Input string in UTF-8. Must be NUL-terminated
 * @param[out]      output  Output buffer
 * @param[in,out]   len     Length of output buffer
 *
 * @returns 0 on success, -EINVAL on invalid parameters, -E2BIG if the buffer
 * filled up before converting the entire string.
 */
int libx52util_convert_utf8_string(const uint8_t *input,
                                   uint8_t *output, size_t *len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* !defined LIBX52UTIL_H */

