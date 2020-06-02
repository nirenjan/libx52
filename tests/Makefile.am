# Automake for libx52 tests
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)
AM_SH_LOG_FLAGS =

TESTS = \
	libx52/test_leds.sh \
	libx52/test_brightness.sh \
	libx52/test_indicator.sh \
	libx52/test_mfd.sh \
	libx52/test_clock.sh \
	libx52/test_timezone.sh

# NOTE: Timezone tests currently fail when local time is daylight time. Once
# this is fixed, remove the line below.
XFAIL_TESTS = libx52/test_timezone.sh

EXTRA_DIST = common_infra.sh $(TESTS)

