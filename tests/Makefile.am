# Automake for libx52 tests
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

# Source: https://stackoverflow.com/a/20831065
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh

TESTS = \
	libx52/test_leds \
	libx52/test_brightness \
	libx52/test_indicator \
	libx52/test_mfd \
	libx52/test_clock \
	libx52/test_timezone

EXTRA_DIST = common_infra.sh $(TESTS)

