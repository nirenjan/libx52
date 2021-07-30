# Automake for x52cli
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

bin_PROGRAMS += x52cli

# Command line utility that front ends the core library
x52cli_SOURCES = cli/x52_cli.c
x52cli_CFLAGS = -I $(top_srcdir)/libx52 $(WARN_CFLAGS)
x52cli_LDFLAGS = $(WARN_LDFLAGS)
x52cli_LDADD = libx52.la

# TODO: Automake doesn't recognize these tests as scripts, and looks for
# C sources. We need to replace these tests with a CMocka test suite
CLI_TESTS = \
	cli/cli_test_leds \
	cli/cli_test_brightness \
	cli/cli_test_indicator \
	cli/cli_test_mfd \
	cli/cli_test_clock \
	cli/cli_test_timezone

EXTRA_DIST += cli/common_infra.sh $(CLI_TESTS)
