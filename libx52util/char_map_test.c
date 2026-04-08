/*
 * X52 character map lookup test
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <libx52/libx52util.h>

#define CHAR_MAP_MAGIC "X52M"
#define CHAR_MAP_HEADER_V1_BYTES 16

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] << 8) | p[1];
}

static uint32_t read_be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
            (uint32_t)p[2] << 8 | p[3];
}

static int parse_char_map_header(const uint8_t *base, size_t filesize,
                                 size_t *data_offset, uint32_t *num_entries,
                                 uint16_t *record_size)
{
    uint16_t version;
    uint16_t rec_sz;
    uint32_t nent;
    uint32_t off;

    if (filesize < CHAR_MAP_HEADER_V1_BYTES) {
        puts("# char_map.bin smaller than header");
        return -1;
    }
    if (memcmp(base, CHAR_MAP_MAGIC, 4) != 0) {
        puts("# char_map.bin missing X52M magic");
        return -1;
    }
    version = read_be16(base + 4);
    rec_sz = read_be16(base + 6);
    nent = read_be32(base + 8);
    off = read_be32(base + 12);
    if (version != 1) {
        printf("# char_map.bin unsupported version %u\n", version);
        return -1;
    }
    if (rec_sz < 2 || rec_sz > 256) {
        printf("# char_map.bin invalid record_size %u\n", rec_sz);
        return -1;
    }
    if (off < CHAR_MAP_HEADER_V1_BYTES || off > filesize) {
        puts("# char_map.bin invalid data_offset");
        return -1;
    }
    if (off + (size_t)nent * rec_sz > filesize) {
        puts("# char_map.bin truncated payload");
        return -1;
    }

    *data_offset = off;
    *num_entries = nent;
    *record_size = rec_sz;
    return 0;
}

// Blindly encode a string into it's smallest UTF8 representation
static void encode_utf8(uint32_t cp, uint8_t *out)
{
    if (cp <= 0x7F) {
        out[0] = (uint8_t)cp;
    } else if (cp <= 0x7FF) {
        out[0] = (uint8_t)(0xC0 | (cp >> 6));
        out[1] = (uint8_t)(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) {
        out[0] = (uint8_t)(0xE0 | (cp >> 12));
        out[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (uint8_t)(0x80 | (cp & 0x3F));
    } else if (cp <= 0x1FFFFF) {
        out[0] = (uint8_t)(0xF0 | (cp >> 18));
        out[1] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        out[2] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[3] = (uint8_t)(0x80 | (cp & 0x3F));
    } else if (cp <= 0x3FFFFFF) {
        out[0] = (uint8_t)(0xF8 | (cp >> 24));
        out[1] = (uint8_t)(0x80 | ((cp >> 18) & 0x3F));
        out[2] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        out[3] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[4] = (uint8_t)(0x80 | (cp & 0x3F));
    } else if (cp <= 0x7FFFFFFF) {
        out[0] = (uint8_t)(0xFC | (cp >> 30));
        out[1] = (uint8_t)(0x80 | ((cp >> 24) & 0x3F));
        out[2] = (uint8_t)(0x80 | ((cp >> 18) & 0x3F));
        out[3] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        out[4] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[5] = (uint8_t)(0x80 | (cp & 0x3F));
    } else { // 0x80000000 to 0xFFFFFFFF (7 bytes)
        out[0] = (uint8_t)0xFE; // Binary 11111110
        out[1] = (uint8_t)(0x80 | ((cp >> 30) & 0x3F));
        out[2] = (uint8_t)(0x80 | ((cp >> 24) & 0x3F));
        out[3] = (uint8_t)(0x80 | ((cp >> 18) & 0x3F));
        out[4] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        out[5] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[6] = (uint8_t)(0x80 | (cp & 0x3F));
    }
}

static double get_time_diff(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[])
{
    uint8_t input[8] = {0};
    uint8_t output[256];
    size_t len;
    int result;

    int fd;
    uint8_t *mapped;
    size_t filesize;
    size_t data_offset;
    uint32_t num_entries;
    uint16_t record_size;
    const uint8_t *expected_blob;
    bool smp_pages_ok;

    struct timespec start, end;
    struct stat st;

    // Argument check
    if (argc != 2) {
        puts("Bail out! Invalid number of arguments");
        puts("# Usage: libx52util-bmp-test <path-to-bin>");
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Bail out! Error %d opening bin file %s: %s\n",
                errno, argv[1], strerror(errno));
        return 1;
    }

    if (fstat(fd, &st) != 0) {
        printf("Bail out! fstat failed %d: %s\n", errno, strerror(errno));
        close(fd);
        return 1;
    }

    filesize = (size_t)st.st_size;
    mapped = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        printf("Bail out! MMAP failed with error %d: %s\n",
                errno, strerror(errno));
        close(fd);
        return 1;
    }

    if (parse_char_map_header(mapped, filesize,
            &data_offset, &num_entries, &record_size) != 0) {
        puts("Bail out! Invalid char_map.bin header");
        munmap(mapped, filesize);
        close(fd);
        return 1;
    }

    if (num_entries < 0x10000) {
        printf("Bail out! num_entries %u < 0x10000\n", num_entries);
        munmap(mapped, filesize);
        close(fd);
        return 1;
    }

    expected_blob = mapped + data_offset;

    puts("TAP version 13");
    // Check the 256 BMP Pages, plus the supplementary pages
    puts("1..257");

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (uint32_t page = 0; page < 256; page++) {
        bool page_ok = true;

        for (uint32_t offset = 0; offset < 256; offset++) {
            uint32_t cp = page * 256 + offset;
            const uint8_t *rec = &expected_blob[cp * record_size];

            memset(input, 0, sizeof(input));
            memset(output, 0, sizeof(output));
            encode_utf8(cp, input);
            len = sizeof(output);

            result = libx52util_convert_utf8_string(input, output, &len);
            if (result != 0) {
                page_ok = false;
                printf("# Bad result @ %04X: %d\n", cp, result);
                break;
            }

            // result is OK, check against the expected blob
            if (len != rec[0]) {
                page_ok = false;
                printf("# Length mismatch @ %04X: expected %u, got %zu\n",
                        cp, rec[0], len);
                break;
            }

            // Length is OK, check the bytes
            if (memcmp(output, &rec[1], rec[0]) != 0) {
                page_ok = false;
                printf("# Output mismatch @ %04X:\n", cp);
                printf("# exp/got:");
                for (size_t i = 0; i < len; i++) {
                    printf("%02X/%02X ", rec[i+1], output[i]);
                }
                puts("");
                break;
            }
        }

        printf("%sok - %d Page 0x%02x\n", page_ok ? "": "not ",
                page + 1, page);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    {
        double time_spent = get_time_diff(start, end);
        printf("# -- Benchmark results --\n");
        printf("# Total time for 64K lookups: %.4f seconds\n", time_spent);
        printf("# Throughput: %.2f Mchars/sec\n", (65536.0 / time_spent) / 1e6);

        printf("# -----------------------\n");
    }

    // Handle the supplementary pages
    smp_pages_ok = true;
    for (uint32_t smp = 0x1; smp <= 0x10; smp++) {
        const uint8_t *rec = &expected_blob[0xFFFD * record_size];
        for (uint32_t offset = 0; offset < 0x100; offset += 0xFF) {
            uint32_t cp = smp * 0x10000 + offset;

            memset(input, 0, sizeof(input));
            memset(output, 0, sizeof(output));
            encode_utf8(cp, input);
            len = sizeof(output);

            result = libx52util_convert_utf8_string(input, output, &len);
            if (result != 0) {
                smp_pages_ok = false;
                printf("# Bad result @ %08X: %d\n", cp, result);
                break;
            }

            // result is OK, check against the expected blob
            if (len != rec[0]) {
                smp_pages_ok = false;
                printf("# Length mismatch @ %08X: expected %u, got %zu\n",

                        cp, rec[0], len);
                break;
            }

            // Length is OK, check the bytes
            if (memcmp(output, &rec[1], rec[0]) != 0) {
                smp_pages_ok = false;
                printf("# Output mismatch @ %08X:\n", cp);
                printf("# exp/got:");
                for (size_t i = 0; i < len; i++) {
                    printf("%02X/%02X ", rec[i+1], output[i]);
                }
                puts("");
                break;
            }
        }

        if (!smp_pages_ok) {
            break;
        }
    }
    printf("%sok - 257 SMP tests\n", smp_pages_ok ? "" : "not ");

    // Cleanup
    munmap(mapped, filesize);
    close(fd);
    return 0;
}
