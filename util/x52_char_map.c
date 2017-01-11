/*
 * Saitek X52 Pro Character Map
 *
 * This file implements functions to perform a lookup of a UCS-4 character
 * in the lookup table.
 *
 * Copyright (C) 2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#include <stdint.h>
#include <errno.h>

#include "libx52util.h"

/**
 * @brief Return the length of the UTF-8 byte sequence
 *
 * This function takes the starting character of a UTF-8 byte sequence
 * and returns the expected length of the sequence in order to convert
 * the sequence to a Unicode codepoint
 *
 * @param   start   Starting character of UTF-8 byte sequence
 *
 * @returns the length of the UTF-8 byte sequence, -EINVAL on invalid character
 */
static inline int32_t _utf8_len(uint8_t start)
{
    if ((start & 0x80) == 0) {
        return 1;
    } else if ((start & 0xe0) == 0xc0) {
        return 2;
    } else if ((start & 0xf0) == 0xe0) {
        return 3;
    } else if ((start & 0xf8) == 0xf0) {
        return 4;
    } else if ((start & 0xfc) == 0xf8) {
        return 5;
    } else if ((start & 0xfe) == 0xfc) {
        return 6;
    } else {
        return -EINVAL;
    }
}

/** @brief Convert a UTF-8 byte sequence to a Unicode codepoint
 *
 * If the first byte is not a valid starting byte, then the byte sequence is
 * treated as being a single byte. However, if an invalid character is
 * encountered during decoding, then the length is the number of bytes from the
 * start of the sequence including the invalid character.
 *
 * @param[in]   str     UTF-8 byte sequence to convert
 * @param[out]  len     Pointer to save the length of the byte sequence
 *
 * @returns the Unicode codepoint for the sequence, -EINVAL on an invalid
 * byte sequence, or invalid length. 
 */
static inline int32_t _utf8_to_unicode(const uint8_t *str, uint32_t *len)
{
    int32_t comp_len = _utf8_len(str[0]);
    int32_t actual_len = 1;
    int32_t codepoint;

    switch (comp_len) {
    case 1:
        codepoint = str[0];
        break;

    case 2:
        codepoint = str[0] & 0x1f;
        break;

    case 3:
        codepoint = str[0] & 0x0f;
        break;

    case 4:
        codepoint = str[0] & 0x07;
        break;

    case 5:
        codepoint = str[0] & 0x03;
        break;

    case 6:
        codepoint = str[0] & 0x01;
        break;

    default:
        codepoint = -EINVAL;
        goto err_return;
    }

    /* Add the additional UTF-8 characters */
    for (; actual_len < comp_len; actual_len++) {
        uint8_t chr = str[actual_len];
        if (chr >= 0x80 && chr <= 0xBF) {
            /* Valid UTF-8 continuation byte */
            codepoint <<= 6;
            codepoint |= chr & 0x3f;
        } else {
            codepoint = -EINVAL;
            break;
        }
    }

err_return:
    *len = actual_len;
    return codepoint;
}

/**
 * @brief Fixed map from Unicode codepoint to X52 character map
 *
 * @param   chr     Unicode codepoint to map to X52 character map
 * @param   unrec   Code point to display if not matching any known entry.
 *                  A negative value will drop the character
 *
 * @returns mapped character, or unrec if not found.
 */
static int _unicode_to_x52(int32_t chr, int unrec)
{
    switch (chr) {
    case 0x0020:
        return 0x0020;

    case 0x0021:
        return 0x0021;

    case 0x0022:
        return 0x0022;

    case 0x0023:
        return 0x0023;

    case 0x0024:
        return 0x0024;

    case 0x0025:
        return 0x0025;

    case 0x0026:
        return 0x0026;

    case 0x0027:
        return 0x0027;

    case 0x0028:
        return 0x0028;

    case 0x0029:
        return 0x0029;

    case 0x002A:
        return 0x002A;

    case 0x002B:
        return 0x002B;

    case 0x002C:
        return 0x002C;

    case 0x002D:
        return 0x002D;

    case 0x002E:
        return 0x002E;

    case 0x002F:
        return 0x002F;

    case 0x0030:
        return 0x0030;

    case 0x0031:
        return 0x0031;

    case 0x0032:
        return 0x0032;

    case 0x0033:
        return 0x0033;

    case 0x0034:
        return 0x0034;

    case 0x0035:
        return 0x0035;

    case 0x0036:
        return 0x0036;

    case 0x0037:
        return 0x0037;

    case 0x0038:
        return 0x0038;

    case 0x0039:
        return 0x0039;

    case 0x003A:
        return 0x003A;

    case 0x003B:
        return 0x003B;

    case 0x003C:
        return 0x003C;

    case 0x003D:
        return 0x003D;

    case 0x003E:
        return 0x003E;

    case 0x003F:
        return 0x003F;

    case 0x0040:
        return 0x0040;

    case 0x0041:
        return 0x0041;

    case 0x0042:
        return 0x0042;

    case 0x0043:
        return 0x0043;

    case 0x0044:
        return 0x0044;

    case 0x0045:
        return 0x0045;

    case 0x0046:
        return 0x0046;

    case 0x0047:
        return 0x0047;

    case 0x0048:
        return 0x0048;

    case 0x0049:
        return 0x0049;

    case 0x004A:
        return 0x004A;

    case 0x004B:
        return 0x004B;

    case 0x004C:
        return 0x004C;

    case 0x004D:
        return 0x004D;

    case 0x004E:
        return 0x004E;

    case 0x004F:
        return 0x004F;

    case 0x0050:
        return 0x0050;

    case 0x0051:
        return 0x0051;

    case 0x0052:
        return 0x0052;

    case 0x0053:
        return 0x0053;

    case 0x0054:
        return 0x0054;

    case 0x0055:
        return 0x0055;

    case 0x0056:
        return 0x0056;

    case 0x0057:
        return 0x0057;

    case 0x0058:
        return 0x0058;

    case 0x0059:
        return 0x0059;

    case 0x005A:
        return 0x005A;

    case 0x005B:
        return 0x005B;

    case 0x005D:
        return 0x005D;

    case 0x005E:
        return 0x005E;

    case 0x005F:
        return 0x005F;

    case 0x0060:
        return 0x0060;

    case 0x0061:
        return 0x0061;

    case 0x0062:
        return 0x0062;

    case 0x0063:
        return 0x0063;

    case 0x0064:
        return 0x0064;

    case 0x0065:
        return 0x0065;

    case 0x0066:
        return 0x0066;

    case 0x0067:
        return 0x0067;

    case 0x0068:
        return 0x0068;

    case 0x0069:
        return 0x0069;

    case 0x006A:
        return 0x006A;

    case 0x006B:
        return 0x006B;

    case 0x006C:
        return 0x006C;

    case 0x006D:
        return 0x006D;

    case 0x006E:
        return 0x006E;

    case 0x006F:
        return 0x006F;

    case 0x0070:
        return 0x0070;

    case 0x0071:
        return 0x0071;

    case 0x0072:
        return 0x0072;

    case 0x0073:
        return 0x0073;

    case 0x0074:
        return 0x0074;

    case 0x0075:
        return 0x0075;

    case 0x0076:
        return 0x0076;

    case 0x0077:
        return 0x0077;

    case 0x0078:
        return 0x0078;

    case 0x0079:
        return 0x0079;

    case 0x007A:
        return 0x007A;

    case 0x007B:
        return 0x007B;

    case 0x007C:
        return 0x007C;

    case 0x007D:
        return 0x007D;


    // Miscellaneous Symbols
    case 0x00A7: /* SECTION SIGN */
        return 0x12;

    case 0x00B6: /* PILCROW SIGN */
        return 0x13;

    case 0x00A9: /* (C) */
        return 0x0F;

    case 0x00AE: /* (R) */
        return 0x0E;


    // Mathematical Symbols
    case 0x00BD: /* VULGAR FRACTION ONE HALF */
        return 0xF5;

    case 0x00BC: /* VULGAR FRACTION ONE QUARTER */
        return 0xF6;

    case 0x00D7: /* MULTIPLICATION SIGN */
        return 0xF7;

    case 0x00F7: /* DIVISION SIGN */
        return 0xF8;

    case 0x2264: /* LESS-THAN OR EQUAL TO */
        return 0xF9;

    case 0x2265: /* GREATER-THAN OR EQUAL TO */
        return 0xFA;

    case 0x226A: /* MUCH LESS-THAN */
        return 0xFB;

    case 0x226B: /* MUCH GREATER-THAN */
        return 0xFC;

    case 0x2260: /* NOT EQUAL TO */
        return 0xFD;

    case 0x221A: /* SQUARE ROOT */
        return 0xFE;


    // Accented Latin characters
    case 0x00C7: /* LATIN CAPITAL LETTER C WITH CEDILLA */
        return 0x80;

    case 0x00FC: /* LATIN SMALL LETTER U WITH DIAERESIS */
        return 0x81;

    case 0x00E9: /* LATIN SMALL LETTER E WITH ACUTE */
        return 0x82;

    case 0x00E2: /* LATIN SMALL LETTER A WITH CIRCUMFLEX */
        return 0x83;

    case 0x00E4: /* LATIN SMALL LETTER A WITH DIAERESIS */
        return 0x84;

    case 0x00E0: /* LATIN SMALL LETTER A WITH GRAVE */
        return 0x85;

    case 0x0227: /* LATIN SMALL LETTER A WITH DOT ABOVE */
        return 0x86;

    case 0x00E7: /* LATIN SMALL LETTER C WITH CEDILLA */
        return 0x87;

    case 0x00EA: /* LATIN SMALL LETTER E WITH CIRCUMFLEX */
        return 0x88;

    case 0x00EB: /* LATIN SMALL LETTER E WITH DIAERESIS */
        return 0x89;

    case 0x00E8: /* LATIN SMALL LETTER E WITH GRAVE */
        return 0x8A;

    case 0x00EF: /* LATIN SMALL LETTER I WITH DIAERESIS */
        return 0x8B;

    case 0x00EE: /* LATIN SMALL LETTER I WITH CIRCUMFLEX */
        return 0x8C;

    case 0x00EC: /* LATIN SMALL LETTER I WITH GRAVE */
        return 0x8D;

    case 0x00C4: /* LATIN CAPITAL LETTER A WITH DIAERESIS */
        return 0x8E;

    case 0x00C2: /* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
        return 0x8F;


    case 0x00C9: /* LATIN CAPITAL LETTER E WITH ACUTE */
        return 0x90;

    case 0x00E6: /* LATIN SMALL LETTER AE */
        return 0x91;

    case 0x00C6: /* LATIN CAPITAL LETTER AE */
        return 0x92;

    case 0x00F4: /* LATIN SMALL LETTER O WITH CIRCUMFLEX */
        return 0x93;

    case 0x00F6: /* LATIN SMALL LETTER O WITH DIAERESIS */
        return 0x94;

    case 0x00F2: /* LATIN SMALL LETTER O WITH GRAVE */
        return 0x95;

    case 0x00FB: /* LATIN SMALL LETTER U WITH CIRCUMFLEX */
        return 0x96;

    case 0x00F9: /* LATIN SMALL LETTER U WITH GRAVE */
        return 0x97;

    case 0x00FF: /* LATIN SMALL LETTER Y WITH DIAERESIS */
        return 0x98;

    case 0x00D6: /* LATIN CAPITAL LETTER O WITH DIAERESIS */
        return 0x99;

    case 0x00DC: /* LATIN CAPITAL LETTER U WITH DIAERESIS */
        return 0x9A;

    case 0x00F1: /* LATIN SMALL LETTER N WITH TILDE */
        return 0x9B;

    case 0x00D1: /* LATIN CAPITAL LETTER N WITH TILDE */
        return 0x9C;

    case 0x00AA: /* FEMININE ORDINAL INDICATOR */
        return 0x9D;

    case 0x00BA: /* MASCULINE ORDINAL INDICATOR */
        return 0x9E;

    case 0x00BF: /* INVERTED QUESTION MARK */
        return 0x9F;


    case 0x00E1: /* LATIN SMALL LETTER A WITH ACUTE */
        return 0xE0;

    case 0x00ED: /* LATIN SMALL LETTER I WITH ACUTE */
        return 0xE1;

    case 0x00F3: /* LATIN SMALL LETTER O WITH ACUTE */
        return 0xE2;

    case 0x00FA: /* LATIN SMALL LETTER U WITH ACUTE */
        return 0xE3;

    case 0x00A2: /* CENT SIGN */
        return 0xE4;

    case 0x00A3: /* POUND SIGN */
        return 0xE5;

    case 0x00A5: /* YEN SIGN */
        return 0xE6;

    // case 0x0000: /* This looks like a Pt, I think this is the Pesata symbol? */
    //    return 0xE7;

    // case 0x0000: /* This looks like a stylized lowercase F. */
    //    return 0xE8;

    case 0x00A1: /* INVERTED EXCLAMATION MARK */
        return 0xE9;

    case 0x00C3: /* LATIN CAPITAL LETTER A WITH TILDE */
        return 0xEA;

    case 0x00E3: /* LATIN SMALL LETTER A WITH TILDE */
        return 0xEB;

    case 0x00D5: /* LATIN CAPITAL LETTER O WITH TILDE */
        return 0xEC;

    case 0x00F5: /* LATIN SMALL LETTER O WITH TILDE */
        return 0xED;

    case 0x00D8: /* LATIN CAPITAL LETTER O WITH STROKE */
        return 0xEE;

    case 0x00F8: /* LATIN SMALL LETTER O WITH STROKE */
        return 0xEF;


    // Greek
    case 0x0393: /* GREEK CAPITAL LETTER GAMMA */
        return 0x14;

    case 0x0394: /* GREEK CAPITAL LETTER DELTA */
        return 0x15;

    case 0x0398: /* GREEK CAPITAL LETTER THETA */
        return 0x16;

    case 0x039B: /* GREEK CAPITAL LETTER LAMDA */
        return 0x17;

    case 0x039E: /* GREEK CAPITAL LETTER XI */
        return 0x18;

    case 0x03A0: /* GREEK CAPITAL LETTER PI */
        return 0x19;

    case 0x03A3: /* GREEK CAPITAL LETTER SIGMA */
        return 0x1A;

    case 0x03D2: /* GREEK UPSILON WITH HOOK SYMBOL */
        return 0x1B;

    case 0x03A6: /* GREEK CAPITAL LETTER PHI */
        return 0x1C;

    case 0x03A8: /* GREEK CAPITAL LETTER PSI */
        return 0x1D;

    case 0x03A9: /* GREEK CAPITAL LETTER OMEGA */
        return 0x1E;

    case 0x03B1: /* GREEK SMALL LETTER ALPHA */
        return 0x1F;


    // Box Drawing
    case 0x250C: /* BOX DRAWINGS LIGHT DOWN AND RIGHT */
        return 0x09;

    case 0x2510: /* BOX DRAWINGS LIGHT DOWN AND LEFT */
        return 0x0A;

    case 0x2514: /* BOX DRAWINGS LIGHT UP AND RIGHT */
        return 0x0B;

    case 0x2518: /* BOX DRAWINGS LIGHT UP AND LEFT */
        return 0x0C;

    case 0x2500: /* BOX DRAWINGS LIGHT HORIZONTAL */
        return 0xFF;


    // TODO: Japanese Kana

    default:
        return unrec;
    }
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
    uint32_t chr_len;
    int chr;
    int retval = 0;

    if (!input || !output || !len || !*len) {
        return -EINVAL;
    }

    index = 0;
    while (*input) {
        chr = _unicode_to_x52(_utf8_to_unicode(input, &chr_len), -1);
        input += chr_len;

        if (chr >= 0) {
            output[index] = chr;
            index++;
            if (index >= *len) {
                retval = -E2BIG;
                break;
            }
        }
    }

    *len = index;
    return retval;
}

