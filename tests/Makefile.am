# Automake for x52cli tests
#
# Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

# Source: https://stackoverflow.com/a/20831065
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh

TESTS = \
	x52cli/test_leds \
	x52cli/test_brightness \
	x52cli/test_indicator \
	x52cli/test_mfd \
	x52cli/test_clock \
	x52cli/test_timezone

EXTRA_DIST = common_infra.sh $(TESTS)

