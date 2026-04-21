/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "x52_test_args.h"
#include <string.h>

void x52test_parse_argv(int argc, char **argv, const struct x52test_cmd *cmds,
                        int test_all_bitmap, struct x52test_argv_result *out)
{
    int i;
    const struct x52test_cmd *c;

    out->status = X52TEST_ARGV_OK;
    out->bad_arg_index = 0;

    if (argc == 1) {
        out->test_bitmap = test_all_bitmap;
        return;
    }

    out->test_bitmap = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "help") || !strcmp(argv[i], "--help")) {
            out->status = X52TEST_ARGV_HELP;
            return;
        }

        int found = 0;

        for (c = cmds; c->cmd; c++) {
            if (!strcmp(argv[i], c->cmd)) {
                out->test_bitmap |= c->test_bitmap;
                found = 1;
                break;
            }
        }

        if (!found) {
            out->status = X52TEST_ARGV_UNKNOWN;
            out->bad_arg_index = i;
            return;
        }
    }
}
