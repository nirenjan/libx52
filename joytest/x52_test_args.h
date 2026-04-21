/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52_TEST_ARGS_H
#define X52_TEST_ARGS_H

enum x52test_argv_status {
    X52TEST_ARGV_OK,
    X52TEST_ARGV_HELP,
    X52TEST_ARGV_UNKNOWN,
};

struct x52test_cmd {
    const char *cmd;
    int test_bitmap;
};

struct x52test_argv_result {
    enum x52test_argv_status status;
    int test_bitmap;
    int bad_arg_index;
};

void x52test_parse_argv(int argc, char **argv, const struct x52test_cmd *cmds,
                        int test_all_bitmap, struct x52test_argv_result *out);

#endif /* X52_TEST_ARGS_H */
