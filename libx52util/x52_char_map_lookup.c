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
#include <errno.h>

#include "libx52util.h"
#include "x52_char_map.h"

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
    struct map_entry *entry;
    size_t index;
    int retval = 0;
    unsigned char local_index;

    if (!input || !output || !len || !*len) {
        return -EINVAL;
    }

    index = 0;
    entry = &map_root[*input];
    while (*input) {
        input++;
        if (entry->type == TYPE_ENTRY) {
            output[index] = entry->value;
            index++;
            if (index >= *len) {
                retval = -E2BIG;
                break;
            }
            entry = &map_root[*input];
        } else if (entry->type == TYPE_POINTER) {
            local_index = *input;
            if (local_index < 0x80 || local_index >= 0xC0) {
                /* Invalid input, skip till we find the start of another
                 * valid UTF-8 character
                 */
                while (*input >= 0x80 && *input < 0xC0) {
                    input++; /* Skip invalid characters */
                }

                /* New UTF-8 character, reset the entry pointer */
                entry = &map_root[*input];
            } else {
                /* Mask off the upper bits, we only care about the lower 6 bits */
                local_index &= 0x3F;
                entry = &(entry->next[local_index]);
            }
        } else {
            /* Invalid value, skip */
            while (*input >= 0x80 && *input < 0xC0) {
                input++; /* Skip invalid characters */
            }
        }
    }

    *len = index;
    return retval;
}

