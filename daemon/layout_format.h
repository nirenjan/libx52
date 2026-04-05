/*
 * Saitek X52 Pro MFD & LED driver - x52layout v1 binary format
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file layout_format.h
 * @brief On-disk keyboard layout (@c .x52l) v1: constants, documentation, load and lookup API.
 *
 * The file is a dense index: entry @c entries[c] maps Unicode scalar @c c when
 * @c 0 <= c < codepoint_limit. For @c c >= codepoint_limit there is no mapping.
 *
 * **Header (128 bytes, all multi-byte integers big-endian / network order):**
 * - **0..3:** magic @c 'X' @c '5' @c '2' @c 'L'
 * - **4..5:** @c version (must be @ref X52_LAYOUT_FORMAT_VERSION)
 * - **6..7:** @c flags (v1: only @ref X52_LAYOUT_FLAG_NAME_TRUNCATED and/or
 *   @ref X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED; other bits are reserved and must be zero)
 * - **8..11:** @c codepoint_limit — exclusive end of range; number of two-byte rows in @c entries
 * - **12..15:** @c checksum — CRC-32 (ZIP/IEEE, same as Python @c zlib.crc32) over the full
 *   file with bytes 12..15 taken as zero when computing the digest
 * - **16..47:** @c layout_name (required: at least one character before @c NUL; remainder zero)
 * - **48..111:** @c description (optional, NUL-terminated, remainder zero)
 * - **112..127:** reserved (ignored on read in v1)
 *
 * **128+:** @c entries[] — pairs @c (modifiers, usage_key) for HID page 0x07; @c (0, 0) is empty.
 *
 * **File size:** exactly @c 128 + 2 * @c codepoint_limit bytes.
 */
#ifndef X52D_LAYOUT_FORMAT_H
#define X52D_LAYOUT_FORMAT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Four-byte magic at offset 0 (not NUL-terminated in the file). */
#define X52_LAYOUT_MAGIC_0 'X'
#define X52_LAYOUT_MAGIC_1 '5'
#define X52_LAYOUT_MAGIC_2 '2'
#define X52_LAYOUT_MAGIC_3 'L'

/** Total header size and byte offset of the @c entries table. */
#define X52_LAYOUT_HEADER_BYTES 128u

#define X52_LAYOUT_FORMAT_VERSION 1u

/** Inclusive maximum for @c codepoint_limit - 1 (Unicode scalar space + sentinel ceiling). */
#define X52_LAYOUT_CODEPOINT_LIMIT_MAX 0x110000u

/** @c layout_name field size in the header (offset 16). */
#define X52_LAYOUT_NAME_FIELD_BYTES 32u

/** @c description field size in the header (offset 48). */
#define X52_LAYOUT_DESCRIPTION_FIELD_BYTES 64u

/**
 * v1 header @c flags (big-endian on disk). @ref x52_layout_flags returns the host-endian value.
 */
#define X52_LAYOUT_FLAG_NAME_TRUNCATED 1u
#define X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED 2u

/** Bitmask of flags defined for v1; other bits must be zero. */
#define X52_LAYOUT_FLAGS_KNOWN (X52_LAYOUT_FLAG_NAME_TRUNCATED | X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED)

/** Loaded layout snapshot (opaque): full file copy, validated at load time. */
typedef struct x52_layout x52_layout;

/**
 * @brief Read a layout file into a malloc'd snapshot and validate it (no @c mmap).
 *
 * @param path  Path to the @c .x52l file; must not be @c NULL (otherwise @c EINVAL)
 * @param out   On success, receives a new @ref x52_layout; must not be @c NULL; caller must @ref x52_layout_free
 *
 * @returns 0 on success, or a positive @c errno value (@c EINVAL, @c ENOMEM, @c EIO, @c ENOENT, …)
 */
int x52_layout_load_path(const char *path, x52_layout **out);

/**
 * @brief Turn @c Profiles.KeyboardLayout INI value into a safe layout basename.
 *
 * Empty or @c NULL yields @c "us" with @p rejected_out @c false. Values containing @c '/' ,
 * @c '\\' , @c ".." , disallowed characters, or oversize strings are rejected: @p out becomes
 * @c "us" and @p rejected_out is @c true (caller should log once). Requires @p out_sz @c >= 3.
 *
 * Allowed characters: ASCII alphanumeric, @c '_' , @c '-'.
 */
void x52_layout_normalize_keyboard_basename(const char *cfg_value, char *out, size_t out_sz, bool *rejected_out);

/**
 * @brief Build @c <datadir>/x52d/<basename>.x52l into @p path.
 *
 * @returns 0 on success, or @c ENAMETOOLONG if the path does not fit
 */
int x52_layout_join_file_path(char *path, size_t path_sz, const char *datadir, const char *basename);

/**
 * @brief Load @c join(datadir, "x52d", basename + ".x52l") after the same validation as @ref x52_layout_load_path.
 *
 * @returns 0 on success, or a positive @c errno (e.g. @c ENOENT, @c EINVAL, @c ENAMETOOLONG)
 */
int x52_layout_load_datadir(const char *datadir, const char *basename, x52_layout **out);

/**
 * @brief Copy @p data into an owned buffer and validate it.
 *
 * Same validation rules as @ref x52_layout_load_path (magic, version, flags, size, CRC-32, entries,
 * non-empty @c layout_name, etc.).
 *
 * @param data  Layout file bytes; may be @c NULL only if @p len is zero (otherwise @c EINVAL)
 * @param len   Number of bytes in @p data
 * @param out   On success, receives a new @ref x52_layout; must not be @c NULL; caller must @ref x52_layout_free
 *
 * @returns 0 on success, or a positive @c errno value (@c EINVAL, @c ENOMEM, …)
 */
int x52_layout_load_memory(const void *data, size_t len, x52_layout **out);

/**
 * @brief Release a layout loaded by @ref x52_layout_load_path or @ref x52_layout_load_memory.
 *
 * @param layout  Layout to free; @c NULL is a no-op
 */
void x52_layout_free(x52_layout *layout);

/**
 * @brief Exclusive end of the Unicode scalar range covered by @c entries (same as on-disk @c codepoint_limit).
 *
 * Lookups for @c code_point >= this value are not in the table.
 *
 * @param layout  Loaded layout, or @c NULL
 *
 * @returns The limit value, or @c 0 if @p layout is @c NULL
 */
uint32_t x52_layout_codepoint_limit(const x52_layout *layout);

/**
 * @brief Host-endian copy of the on-disk @c flags field at header offset 6.
 *
 * Only bits in @ref X52_LAYOUT_FLAGS_KNOWN may be set in valid files; the loader rejects unknown bits.
 *
 * @param layout  Loaded layout, or @c NULL
 *
 * @returns Flag word, or @c 0 if @p layout is @c NULL
 */
uint16_t x52_layout_flags(const x52_layout *layout);

/**
 * @brief Layout name from the header @c layout_name field.
 *
 * If @ref X52_LAYOUT_FLAG_NAME_TRUNCATED is set, the returned string is the on-disk name plus
 * @c "<truncated>". The pointer remains valid until @ref x52_layout_free; do not modify the string.
 *
 * @param layout  Loaded layout, or @c NULL
 *
 * @returns Read-only NUL-terminated UTF-8 string; empty string if @p layout is @c NULL
 */
const char *x52_layout_name(const x52_layout *layout);

/**
 * @brief Optional description from the header @c description field.
 *
 * If @ref X52_LAYOUT_FLAG_DESCRIPTION_TRUNCATED is set, the returned string is the on-disk text plus
 * @c "<truncated>". The pointer remains valid until @ref x52_layout_free; do not modify the string.
 *
 * @param layout  Loaded layout, or @c NULL
 *
 * @returns Read-only NUL-terminated UTF-8 string, or empty string if @p layout is @c NULL or the field is empty
 */
const char *x52_layout_description(const x52_layout *layout);

/**
 * @brief O(1) lookup of the HID chord for Unicode scalar @p code_point.
 *
 * Returns @c false when @p code_point is out of range, when the slot is empty @c (modifiers, usage_key) = (0, 0),
 * or when any argument is invalid. On @c false, @p modifiers_out and @p usage_key_out are left unchanged.
 *
 * @param layout           Loaded layout; if @c NULL, returns @c false
 * @param code_point       Unicode scalar value
 * @param[out] modifiers_out  HID keyboard modifier byte (@ref vkm_key_modifiers bits); if @c NULL, returns @c false
 * @param[out] usage_key_out  HID usage (page 0x07); if @c NULL, returns @c false
 *
 * @returns @c true if a non-empty mapping exists and was written to @p modifiers_out and @p usage_key_out
 */
bool x52_layout_lookup(const x52_layout *layout, uint32_t code_point, uint8_t *modifiers_out,
                       uint8_t *usage_key_out);

#ifdef __cplusplus
}
#endif

#endif /* X52D_LAYOUT_FORMAT_H */
