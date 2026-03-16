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
#include <unistd.h>

#include "libx52util.h"

// Fix this if we ever hit longer sequences
#define RECORD_SIZE 8

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

int main(int argc, char *argv[])
{
    uint8_t input[8] = {0};
    uint8_t output[RECORD_SIZE];
    size_t len;
    int result;

    int fd;
    uint8_t *expected_blob;
    bool smp_pages_ok;

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

    expected_blob = mmap(NULL, 0x10000 * RECORD_SIZE,
                         PROT_READ, MAP_SHARED, fd, 0);
    if (expected_blob == MAP_FAILED) {
        printf("Bail out! MMAP failed with error %d: %s\n",
                errno, strerror(errno));
    }

    puts("TAP version 13");
    // Check the 256 BMP Pages, plus the supplementary pages
    puts("1..257");

    for (uint32_t page = 0; page < 256; page++) {
        bool page_ok = true;

        for (uint32_t offset = 0; offset < 256; offset++) {
            uint32_t cp = page * 256 + offset;
            const uint8_t *rec = &expected_blob[cp * RECORD_SIZE];

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
                for (int i = 0; i < len; i++) {
                    printf("%02X/%02X ", rec[i+1], output[i]);
                }
                puts("");
                break;
            }
        }

        printf("%sok - %d Page 0x%02x\n", page_ok ? "": "not ",
                page + 1, page);
    }

    // Handle the supplementary pages
    smp_pages_ok = true;
    for (uint32_t smp = 0x1; smp <= 0x10; smp++) {
        const uint8_t *rec = &expected_blob[0xFFFD * RECORD_SIZE];
        for (uint32_t offset = 0; offset < 0x100; offset += 0xFF) {
            uint32_t cp = smp * 256 + offset;

            memset(input, 0, sizeof(input));
            memset(output, 0, sizeof(output));
            len = sizeof(output);
            encode_utf8(cp, input);

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
                for (int i = 0; i < len; i++) {
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
    munmap(expected_blob, 0x10000 * RECORD_SIZE);
    close(fd);
    return 0;
}
