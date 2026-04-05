/*
 * Saitek X52 Pro MFD & LED driver - CRC-32 (zlib / Python zlib.crc32 compatible)
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file crc32.h
 * @brief IEEE/ZIP CRC-32 matching @c zlib.crc32 / Python @c zlib.crc32 (polynomial 0xEDB88320).
 *
 * Incremental use: @code
 *   uint32_t crc = x52_crc32_init();
 *   crc = x52_crc32_update(crc, chunk0, len0);
 *   crc = x52_crc32_update(crc, chunk1, len1);
 *   // crc is final value (unsigned 32-bit, same encoding as Python after & 0xFFFFFFFF)
 * @endcode
 */
#ifndef X52D_CRC32_H
#define X52D_CRC32_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initial accumulator value (same as first argument to zlib @c crc32 for a new stream). */
#define X52_CRC32_INIT 0u

/** Start a new CRC computation. */
uint32_t x52_crc32_init(void);

/**
 * Feed zero or more bytes into a running CRC.
 *
 * @param crc   Value from @ref x52_crc32_init or a prior @ref x52_crc32_update
 * @param data  Input bytes; must be non-NULL if @p len is non-zero
 * @param len   Number of bytes
 * @return      Updated CRC-32 (same as Python <tt>zlib.crc32(data, crc) & 0xFFFFFFFF</tt>
 *              when @p data is the new chunk only)
 */
uint32_t x52_crc32_update(uint32_t crc, const void *data, size_t len);

/** Alias for @ref x52_crc32_update (zlib-style name). */
static inline uint32_t x52_crc32(uint32_t crc, const void *data, size_t len)
{
    return x52_crc32_update(crc, data, len);
}

#ifdef __cplusplus
}
#endif

#endif /* X52D_CRC32_H */
