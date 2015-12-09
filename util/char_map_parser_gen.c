/*
 * Saitek X52 Pro Character Map Parser and Generator
 *
 * This file takes in an input map and converts it to a C file which implements
 * a lookup table to match the input UTF-8 character to the corresponding
 * character map value.
 *
 * Copyright (C) 2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "x52_char_map.h"

struct map_table {
    struct map_table *next;
    uint32_t value_so_far;
    uint16_t type;
    uint8_t value;
};

struct map_table *root;

struct stack_node {
    struct stack_node *next;
    struct map_table *entry;
    uint32_t value;
};

struct stack_node *stack;

/*
 * Convert Unicode code point to UTF-8 bytearray. The bytearray should be
 * at least 8 bytes long to accomodate for the longest possible value of
 * Unicode code point and a terminating NUL byte.
 */
static void convert_unichr_to_utf8(int unicode_cp, char *utf8_bytearray)
{
    if (unicode_cp <= 0x7F) {
        utf8_bytearray[0] = unicode_cp;
        utf8_bytearray[1] = 0;
    } else if (unicode_cp <= 0x7FF) {
        utf8_bytearray[0] = 0xC0 | ((unicode_cp & 0x7C0) >> 6);
        utf8_bytearray[1] = 0x80 |  (unicode_cp & 0x03F);
        utf8_bytearray[2] = 0;
    } else if (unicode_cp <= 0xFFFF) {
        utf8_bytearray[0] = 0xE0 | ((unicode_cp & 0xF000) >> 12);
        utf8_bytearray[1] = 0x80 | ((unicode_cp & 0x0FC0) >> 6);
        utf8_bytearray[2] = 0x80 |  (unicode_cp & 0x003F);
        utf8_bytearray[3] = 0;
    } else if (unicode_cp <= 0x1FFFFF) {
        utf8_bytearray[0] = 0xF0 | ((unicode_cp & 0x1C0000) >> 18);
        utf8_bytearray[1] = 0x80 | ((unicode_cp & 0x03F000) >> 12);
        utf8_bytearray[2] = 0x80 | ((unicode_cp & 0x000FC0) >> 6);
        utf8_bytearray[3] = 0x80 |  (unicode_cp & 0x00003F);
        utf8_bytearray[4] = 0;
    } else if (unicode_cp <= 0x3FFFFFF) {
        utf8_bytearray[0] = 0xF8 | ((unicode_cp & 0x3000000) >> 24);
        utf8_bytearray[1] = 0x80 | ((unicode_cp & 0x0FC0000) >> 18);
        utf8_bytearray[2] = 0x80 | ((unicode_cp & 0x003F000) >> 12);
        utf8_bytearray[3] = 0x80 | ((unicode_cp & 0x0000FC0) >> 6);
        utf8_bytearray[4] = 0x80 |  (unicode_cp & 0x000003F);
        utf8_bytearray[5] = 0;
    } else /* if (unicode_cp <= 0x7FFFFFFF) */ {
        utf8_bytearray[0] = 0xFC | ((unicode_cp & 0x40000000) >> 30);
        utf8_bytearray[1] = 0x80 | ((unicode_cp & 0x3F000000) >> 24);
        utf8_bytearray[2] = 0x80 | ((unicode_cp & 0x00FC0000) >> 18);
        utf8_bytearray[3] = 0x80 | ((unicode_cp & 0x0003F000) >> 12);
        utf8_bytearray[4] = 0x80 | ((unicode_cp & 0x00000FC0) >> 6);
        utf8_bytearray[5] = 0x80 |  (unicode_cp & 0x0000003F);
        utf8_bytearray[6] = 0;
    }
}

static int stack_push(struct map_table *entry, int value)
{
    struct stack_node *node = calloc(1, sizeof(*node));

    if (!node) {
        fprintf(stderr, "Cannot allocate memory for stack node!\n");
        return 0;
    }

    node->entry = entry;
    node->value = value;
    node->next = stack;
    stack = node;

    return 1;
}

static int stack_pop(struct stack_node *node)
{
    struct stack_node *temp;

    if (stack) {
        *node = *stack;

        temp = stack;
        stack = stack->next;
        free(temp);
        return 1;
    }

    return 0;
}

static int add_to_mem(int input_val, int map_val)
{
    uint8_t bytearray[8];
    int i;
    int value_so_far = 0;
    uint8_t c;
    struct map_table *level;
    struct map_table *temp;

    if (!root) {
        root = calloc(256, sizeof(*root));
        if (!root) {
            fprintf(stderr, "Cannot allocate memory for root node!\n");
            return 0;
        }

        if (!stack_push(root, 0)) {
            return 0;
        }
    }

    convert_unichr_to_utf8(input_val, bytearray);
    level = root;
    for (i = 0; bytearray[i]; i++) {
        c = bytearray[i];
        value_so_far <<= 8;
        value_so_far |= c;
        if (bytearray[i+1]) {
            level[c].type = TYPE_POINTER;
            if (!level[c].next) {
                temp = calloc(256, sizeof(*temp));
                if (!temp) {
                    fprintf(stderr, "Cannot allocate memory for entry table!\n");
                    return 0;
                }
                if (!stack_push(temp, value_so_far)) {
                    return 0;
                }
                level[c].value_so_far = value_so_far;
                level[c].next = temp;
            }

            level = level[c].next;
        } else {
            level[c].type = TYPE_ENTRY;
            level[c].value = map_val;
        }
    }

    return 1;
}

/*
 * Parse an input line and return the input value and map value
 * For comment lines, or invalid lines, returned input value is 0.
 *
 * For invalid lines, return value is 0
 * For valid lines, return value is 1
 * For comment lines, return value is 2
 */
static int parse_line(char *line, int *input_value, int *map_value)
{
    const char *format_str1 = " 0x%x 0x%x";
    const char *format_str2 = " 0x%x %c";
    int ret;
    int val1;
    int val2;
    char c;

    /* Strip off leading whitespace */
    while (*line && isspace(*line)) line++;

    /* If line begins with # or is empty, it's a comment line */
    if (line[0] == '#' || line[0] == '\0') {
        /* Comment line */
        *input_value = 0;
        return 2;
    }

    /* Try to match input against format_str1 */
    ret = sscanf(line, format_str1, &val1, &val2);
    if (ret == 2) {
        /* We have a match! */
        *input_value = val1;
        *map_value = val2;
        return 1;
    }

    /* Try to match input against format_str2 */
    ret = sscanf(line, format_str2, &val1, &c);
    if (ret == 2) {
        /* We have a match! */
        *input_value = val1;
        *map_value = c;
        return 1;
    }

    /* Neither format string matched, and it's not a comment line. Abort */
    return 0;
}

static void write_output(FILE *dest)
{
    struct stack_node node;
    int i;
    int start;
    int end;

    while (stack_pop(&node)) {
        if (node.value) {
            fprintf(dest, "static struct map_entry table_%x[64] = {\n", node.value);
            start = 0x80;
            end = 0xC0;
        } else {
            fprintf(dest, "struct map_entry map_root[256] = {\n");
            start = 0;
            end = 256;
        }

        for (i = start; i < end; i++) {
            if (node.entry[i].next) {
                fprintf(dest, "\t[0x%02x] = { table_%x, TYPE_POINTER, 0 },\n",
                    i - start, node.entry[i].value_so_far);
            } else if (node.entry[i].type == TYPE_ENTRY) {
                fprintf(dest, "\t[0x%02x] = { NULL, TYPE_ENTRY, 0x%02x },\n",
                    i - start, node.entry[i].value);
            }
        }

        fprintf(dest, "};\n\n");
    }
}

void dummy(void)
{
    printf("Dummy function for breakpoint trap\n");
}

int main(int argc, char *argv[])
{
    FILE *input = NULL;
    FILE *output = NULL;
    char buffer[512];
    int rc = 0;

    #define EXIT(v) do { rc = (v); goto exit_handler; } while (0)

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input-map> <output-c-file>\n", argv[0]);
        return 1;
    }

    input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Unable to read input file %s\n", argv[1]);
        EXIT(2);
    }

    output = fopen(argv[2], "w");
    if (!output) {
        fprintf(stderr, "Unable to write output file %s\n", argv[2]);
        EXIT(2);
    }

    while (fgets(buffer, sizeof(buffer), input)) {
        int input_val;
        int map_val;
        if (!parse_line(buffer, &input_val, &map_val)) {
            fprintf(stderr, "Invalid line: %s\n", buffer);
            EXIT(3);
        }

        if (!add_to_mem(input_val, map_val)) {
            EXIT(4);
        }
    }

    /* Write header for output file */
    fputs("/*\n", output);
    fputs(" * Autogenerated character map file for Saitek X52 Pro\n", output);
    fputs(" * Generated from ", output);
    fputs(argv[1], output);
    fputs("\n */\n\n", output);
    fputs("#include \"x52_char_map.h\"\n", output);
    fputs("\n\n", output);

    write_output(output);

exit_handler:
    if (output) fclose(output);
    if (input)  fclose(input);
    return rc;
}
