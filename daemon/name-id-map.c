/*
 * Name ID map - needed to map module/loglevel names to numeric v alues
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stddef.h>
#include <string.h>

#include "name-id-map.h"
#include "module-map.h"

static int map_get_id(const struct name_id_map *map, const char *string)
{
    int i;

    for (i = 0; map[i].name != NULL; i++) {
        if (strcasecmp(map[i].name, string) == 0) {
            return map[i].id;
        }
    }

    // We've broken out of the loop, return the current ID
    return map[i].id;
}

static const char *map_get_name(const struct name_id_map *map, int id)
{
    int i;

    for (i = 0; map[i].name != NULL; i++) {
        if (map[i].id == id) {
            return map[i].name;
        }
    }

    return NULL;
}

int lookup_module_by_name(const char *name)
{
    return map_get_id(module_map, name);
}

const char * lookup_module_by_id(int id)
{
    return map_get_name(module_map, id);
}

int lookup_level_by_name(const char *name)
{
    return map_get_id(loglevel_map, name);
}

const char * lookup_level_by_id(int id)
{
    return map_get_name(loglevel_map, id);
}
