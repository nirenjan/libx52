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

if HAVE_CMOCKA
TESTS += test-cli
check_PROGRAMS += test-cli

test_cli_SOURCES = cli/x52_cli.c cli/test_x52_cli.c
test_cli_CFLAGS = -DX52_CLI_TESTING -I $(top_srcdir)/libx52
test_cli_LDFLAGS = @CMOCKA_LIBS@ $(WARN_LDFLAGS)

# Add a dependency on test_x52_cli_tests.c
cli/test_x52_cli.c: cli/test_x52_cli_tests.c
endif

EXTRA_DIST += cli/test_x52_cli_tests.c
