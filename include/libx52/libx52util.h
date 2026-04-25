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
#include <stdint.h>

#ifndef LIBX52UTIL_API
# if defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
#  define LIBX52UTIL_API __attribute__((visibility("default")))
# elif defined(_WIN32)
#  define LIBX52UTIL_API __declspec(dllexport)
# else
#  define LIBX52UTIL_API
# endif
#endif

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
LIBX52UTIL_API int libx52util_convert_utf8_string(const uint8_t *input,
                                                  uint8_t *output, size_t *len);

/**
 * @name MFD line scrolling
 *
 * Step a 16-cell window over a virtual tape of X52 MFD display bytes (same
 * encoding as libx52_set_text / libx52util_convert_utf8_string output). Cell
 * count follows convert semantics (multi-byte glyphs use multiple cells).
 *
 * **Default wrap:** unless #LIBX52UTIL_SCROLL_SINGLE_PASS is set, each
 * libx52util_scroll_next() advances the window modulo the tape period so the
 * marquee loops until libx52util_scroll_reset() or libx52util_scroll_free().
 *
 * These routines are not thread-safe; callers must serialize access to a given
 * libx52util_scroll_state.
 *
 * @{
 */

/** Opaque scroll state allocated by libx52util_scroll_new(). */
typedef struct libx52util_scroll libx52util_scroll_state;

/**
 * @brief Bitwise flags for libx52util_scroll_new().
 *
 * Combine enumerator values with `|`. Each enumerator is a distinct power of two.
 */
typedef enum libx52util_scroll_flags {
    LIBX52UTIL_SCROLL_NONE = 0,
    /** Virtual prefix of 16 ASCII spaces before converted text (scroll-in). */
    LIBX52UTIL_SCROLL_IN = 1u << 0,
    /** Virtual suffix of 16 ASCII spaces after converted text (scroll-out). */
    LIBX52UTIL_SCROLL_OUT = 1u << 1,
    /**
     * Horizontal direction opposite the default ticker (default advances the window
     * so content appears to move left; with this flag, content moves right).
     */
    LIBX52UTIL_SCROLL_LTR = 1u << 2,
    /**
     * Disable looping: after one full cycle over distinct window positions, the
     * window clamps at the end; further libx52util_scroll_next() returns -EAGAIN
     * until libx52util_scroll_reset() or libx52util_scroll_new(). Without this flag,
     * the window wraps continuously. Implies LIBX52UTIL_SCROLL_OUT.
     */
    LIBX52UTIL_SCROLL_SINGLE_PASS = 1u << 3,
} libx52util_scroll_flags;

/**
 * @brief Allocate scroll state from a UTF-8 string.
 *
 * Converts @p utf8_string with libx52util_convert_utf8_string() using an
 * internal 256-byte output buffer. Converted length is in display cells, same
 * as libx52util_convert_utf8_string()'s output length.
 *
 * @param[out]      state        Receives the new state; must not be NULL.
 * @param[in]       utf8_string  NUL-terminated UTF-8 input; must not be NULL.
 * @param[in]       flags        Bitwise OR of #libx52util_scroll_flags values.
 *
 * @returns 0 if the full string fit the internal buffer; -E2BIG if conversion
 * stopped early but @p *state is still a valid object for scroll_next/reset/free
 * (truncated prefix only). On -EINVAL or -ENOMEM, @p *state is left unchanged.
 * -EINVAL: NULL @p state, NULL @p utf8_string, or invalid argument use.
 * -ENOMEM: allocation failed.
 *
 * If the converted text length is at most 16 cells, or the full virtual tape
 * (optional 16-space prefix + text + optional 16-space suffix) is at most 16
 * cells long, there is no window motion: libx52util_scroll_next() returns 0
 * once with a padded line, then -EAGAIN without advancing.
 */
LIBX52UTIL_API int libx52util_scroll_new(libx52util_scroll_state **state,
                                         const uint8_t *utf8_string,
                                         libx52util_scroll_flags flags);

/**
 * @brief Free scroll state.
 *
 * If @p state is non-NULL and @p *state is non-NULL, frees @p *state and sets
 * @p *state to NULL. If @p state is NULL, no operation is performed.
 */
LIBX52UTIL_API void libx52util_scroll_free(libx52util_scroll_state **state);

/**
 * @brief Rewind the scroll window to the initial position.
 *
 * @param[in]       state   Scroll state from libx52util_scroll_new().
 *
 * @returns 0 on success, -EINVAL if @p state is NULL.
 */
LIBX52UTIL_API int libx52util_scroll_reset(libx52util_scroll_state *state);

/**
 * @brief Emit the next 16-byte MFD line for the current window.
 *
 * Writes up to 16 display bytes into @p display (ASCII space padding where the
 * window extends past the virtual tape). Returns 0 when the new frame differs
 * from the last frame returned with 0; updates the last-emitted snapshot and
 * advances the internal window for the following call. Returns -EAGAIN when the
 * candidate frame equals that last snapshot: @p display is not modified, the
 * snapshot is unchanged, but the window index still advances (wrap or
 * single-pass clamp) so uniform runs eventually change at boundaries.
 *
 * When there is no scrolling (see libx52util_scroll_new): the first call
 * returns 0; later calls return -EAGAIN without advancing the window.
 *
 * @param[in]       state    Scroll state; must not be NULL.
 * @param[out]      display  16-byte output buffer; must not be NULL.
 *
 * @returns 0 on a new visible frame, -EAGAIN when unchanged or static repeat,
 * -EINVAL if @p state or @p display is NULL.
 */
LIBX52UTIL_API int libx52util_scroll_next(libx52util_scroll_state *state,
                                           uint8_t display[16]);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* !defined LIBX52UTIL_H */

