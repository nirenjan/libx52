/*
 * Saitek X52 Pro Character Map Lookup
 *
 * This file implements functions to perform a lookup of a UTF-8 character
 * in the generated lookup table.
 *
 * Copyright (C) 2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <libx52/libx52util.h>
#include "x52_char_map.h"

/**
 * @brief Converts a UTF8 stream to a uint32_t
 *
 * @param[in]       utf8in  Pointer to UTF8 input stream. Must be NUL-terminated
 * @param[out]      unichr  Output character pointer
 *
 * @returns number of bytes to advance stream by - 0 if NUL or input pointer is NULL
 */
static int utf8_to_u32(const uint8_t *utf8in, uint32_t *unichr)
{
    uint8_t b;
    if (!utf8in || !*utf8in) return 0;

    b = utf8in[0];

    // 1-byte (0xxxxxxx)
    if (b < 0x80) {
        *unichr = b;
        return 1;
    }

    // Invalid leading bytes
    if (b < 0xC2 || b > 0xF4) goto error;

    // 2-byte (110xxxxx 10xxxxxx)
    if ((b & 0xE0) == 0xC0) {
        if ((utf8in[1] & 0xC0) != 0x80) goto error;
        *unichr = ((b & 0x1F) << 6) | (utf8in[1] & 0x3F);
        return 2;
    }

    // 3-byte (1110xxxx 10xxxxxx 10xxxxxx)
    if ((b & 0xF0) == 0xE0) {
        if ((utf8in[1] & 0xC0) != 0x80 || (utf8in[2] & 0xC0) != 0x80) goto error;
        *unichr = ((b & 0x0F) << 12) | ((utf8in[1] & 0x3F) << 6) | (utf8in[2] & 0x3F);
        return 3;
    }

    // 4-byte (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
    if ((b & 0xF8) == 0xF0) {
        if ((utf8in[1] & 0xC0) != 0x80 || (utf8in[2] & 0xC0) != 0x80 ||
                (utf8in[3] & 0xC0) != 0x80) goto error;
        *unichr = ((b & 0x07) << 18) | ((utf8in[1] & 0x3F) << 12) |
              ((utf8in[2] & 0x3F) << 6) | (utf8in[3] & 0x3F);
        return 4;
    }

error:
    *unichr = 0xFFFD; // Unicode Replacement Character
    return 1;     // Consume lead byte to attempt resync
}

/**
 * @brief Convert UTF8 string to X52 character map.
 *
 * This function takes in a UTF-8 string and converts it to the character
 * map used by the X52Pro MFD. Unrecognized characters are silently dropped.
 *
 * @param[in]       input   Input string in UTF-8. Must be NUL-terminated
 * @param[out]      output  Output buffer
 * @param[inout]    len     Length of output buffer
 *
 * @returns 0 on success, -EINVAL on invalid parameters, -E2BIG if the buffer
 * filled up before converting the entire string.
 */
int libx52util_convert_utf8_string(const uint8_t *input,
                                   uint8_t *output, size_t *len)
{
    size_t index;
    int retval = 0;
    uint32_t unichr;
    int bytes_consumed;
    uint16_t translated;

    if (!input || !output || !len || !*len) {
        return -EINVAL;
    }

    index = 0;
    // Reset the output array
    memset(output, 0, *len);

    while (*input) {
        // Length check
        if (index >= *len) {
            retval = -E2BIG;
            break;
        }

        bytes_consumed = utf8_to_u32(input, &unichr);
        if (bytes_consumed == 0) {
            // We should never get here, since the while loop should have
            // caught it
            retval = 0;
            break;
        }
        input += bytes_consumed;

        // Check for bytes in the Supplementary planes
        if (unichr >= 0x10000) {
            unichr = 0xFFFD; // Unicode replacement character
        }

        translated = root_table[unichr >> 8][unichr & 0xFF];
        if (translated < 256) {
            // Table entry, push to output
            output[index] = (uint8_t)translated;
            index++;
        } else {
            // We have a sequence, output that
            const uint8_t *sequence = sequence_table[translated - 256];
            uint8_t seq_len = sequence[0];

            // Let's make sure that we can actually output to the buffer
            if ((index + seq_len) >= *len) {
                retval = -E2BIG;
                break;
            }

            for (int i = 1; i <= seq_len; i++) {
                output[index] = sequence[i];
                index++;
            }
        }
    }

    *len = index;
    return retval;
}
