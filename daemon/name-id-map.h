/*
 * Name ID map - needed to map module/loglevel names to numeric v alues
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef NAME_ID_MAP_H
#define NAME_ID_MAP_H

struct name_id_map {
    char *name;
    int id;
};

extern const struct name_id_map module_map[];
extern const struct name_id_map loglevel_map[];

#endif // !defined NAME_ID_MAP_H
