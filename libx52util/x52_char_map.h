/*
 * Saitek X52 Pro Character Map
 *
 * Copyright (C) 2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52_CHAR_MAP_H
#define X52_CHAR_MAP_H

#include <stddef.h>
#include <stdint.h>

enum {
    TYPE_INVALID = 0,   /* Invalid type (default) */

    TYPE_POINTER,       /* Pointer target */

    TYPE_ENTRY          /* Map entry value */
};

struct map_entry {
    struct map_entry *next; /* Pointer to the next table */
    uint8_t type;           /* Type of entry */
    uint8_t value;          /* Value is valid if this is of TYPE_ENTRY */
};

extern struct map_entry map_root[];

#endif /* !defined X52_CHAR_MAP_H */
