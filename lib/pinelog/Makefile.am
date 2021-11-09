# Top level Automake for pinelog
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: MIT

ACLOCAL_AMFLAGS = -I m4

# Extra files that need to be in the distribution
EXTRA_DIST = \
	LICENSE \
	README.md \
	pinelog.h

noinst_LTLIBRARIES = libpinelog.la

# pinelog logging library
libpinelog_la_SOURCES = pinelog.c
libpinelog_la_CFLAGS = @PINELOG_CFLAGS@ $(WARN_CFLAGS) -I $(top_builddir)
libpinelog_la_LDFLAGS = $(WARN_LDFLAGS)

test_SRCFILES = test_pinelog.c $(libpinelog_la_SOURCES)
bench_SRCFILES = bench_pinelog.c $(libpinelog_la_SOURCES)
test_CFLAGS = \
	-DPINELOG_FATAL_STR='"F"' \
	-DPINELOG_ERROR_STR='"E"' \
	-DPINELOG_WARNING_STR='"W"' \
	-DPINELOG_INFO_STR='"I"' \
	-DPINELOG_DEBUG_STR='"D"' \
	-DPINELOG_TRACE_STR='"T"' \
	-DPINELOG_DEFAULT_LEVEL=PINELOG_LVL_TRACE \
	-DPINELOG_DEFAULT_STREAM=stderr \
	-DPINELOG_TEST -I $(top_builddir)

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh
TESTS = \
	fp_test_ts_lvl_tr \
	fp_test_ts_lvl_notr \
	fp_test_ts_nolvl_tr \
	fp_test_ts_nolvl_notr \
	fp_test_nots_lvl_tr \
	fp_test_nots_lvl_notr \
	fp_test_nots_nolvl_tr \
	fp_test_nots_nolvl_notr \
	fp_bench_ts_lvl_tr \
	fp_bench_ts_lvl_notr \
	fp_bench_ts_nolvl_tr \
	fp_bench_ts_nolvl_notr \
	fp_bench_nots_lvl_tr \
	fp_bench_nots_lvl_notr \
	fp_bench_nots_nolvl_tr \
	fp_bench_nots_nolvl_notr \
	str_test_ts_lvl_tr \
	str_test_ts_lvl_notr \
	str_test_ts_nolvl_tr \
	str_test_ts_nolvl_notr \
	str_test_nots_lvl_tr \
	str_test_nots_lvl_notr \
	str_test_nots_nolvl_tr \
	str_test_nots_nolvl_notr \
	str_bench_ts_lvl_tr \
	str_bench_ts_lvl_notr \
	str_bench_ts_nolvl_tr \
	str_bench_ts_nolvl_notr \
	str_bench_nots_lvl_tr \
	str_bench_nots_lvl_notr \
	str_bench_nots_nolvl_tr \
	str_bench_nots_nolvl_notr

check_PROGRAMS = $(TESTS)
fp_test_ts_lvl_tr_SOURCES = $(test_SRCFILES)
fp_test_ts_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
fp_test_ts_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_ts_lvl_notr_SOURCES = $(test_SRCFILES)
fp_test_ts_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
fp_test_ts_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_ts_nolvl_tr_SOURCES = $(test_SRCFILES)
fp_test_ts_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
fp_test_ts_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_ts_nolvl_notr_SOURCES = $(test_SRCFILES)
fp_test_ts_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
fp_test_ts_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_nots_lvl_tr_SOURCES = $(test_SRCFILES)
fp_test_nots_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
fp_test_nots_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_nots_lvl_notr_SOURCES = $(test_SRCFILES)
fp_test_nots_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
fp_test_nots_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_nots_nolvl_tr_SOURCES = $(test_SRCFILES)
fp_test_nots_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
fp_test_nots_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_test_nots_nolvl_notr_SOURCES = $(test_SRCFILES)
fp_test_nots_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
fp_test_nots_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_ts_lvl_tr_SOURCES = $(bench_SRCFILES)
fp_bench_ts_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
fp_bench_ts_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_ts_lvl_notr_SOURCES = $(bench_SRCFILES)
fp_bench_ts_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
fp_bench_ts_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_ts_nolvl_tr_SOURCES = $(bench_SRCFILES)
fp_bench_ts_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
fp_bench_ts_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_ts_nolvl_notr_SOURCES = $(bench_SRCFILES)
fp_bench_ts_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
fp_bench_ts_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_nots_lvl_tr_SOURCES = $(bench_SRCFILES)
fp_bench_nots_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
fp_bench_nots_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_nots_lvl_notr_SOURCES = $(bench_SRCFILES)
fp_bench_nots_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
fp_bench_nots_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_nots_nolvl_tr_SOURCES = $(bench_SRCFILES)
fp_bench_nots_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
fp_bench_nots_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

fp_bench_nots_nolvl_notr_SOURCES = $(bench_SRCFILES)
fp_bench_nots_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
fp_bench_nots_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

test_CFLAGS += -DPINELOG_BUFFER_SZ=4096
str_test_ts_lvl_tr_SOURCES = $(test_SRCFILES)
str_test_ts_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
str_test_ts_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_test_ts_lvl_notr_SOURCES = $(test_SRCFILES)
str_test_ts_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
str_test_ts_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_test_ts_nolvl_tr_SOURCES = $(test_SRCFILES)
str_test_ts_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
str_test_ts_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_test_ts_nolvl_notr_SOURCES = $(test_SRCFILES)
str_test_ts_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
str_test_ts_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_test_nots_lvl_tr_SOURCES = $(test_SRCFILES)
str_test_nots_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
str_test_nots_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_test_nots_lvl_notr_SOURCES = $(test_SRCFILES)
str_test_nots_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
str_test_nots_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_test_nots_nolvl_tr_SOURCES = $(test_SRCFILES)
str_test_nots_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
str_test_nots_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_test_nots_nolvl_notr_SOURCES = $(test_SRCFILES)
str_test_nots_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
str_test_nots_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_ts_lvl_tr_SOURCES = $(bench_SRCFILES)
str_bench_ts_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
str_bench_ts_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_ts_lvl_notr_SOURCES = $(bench_SRCFILES)
str_bench_ts_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
str_bench_ts_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_ts_nolvl_tr_SOURCES = $(bench_SRCFILES)
str_bench_ts_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
str_bench_ts_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_ts_nolvl_notr_SOURCES = $(bench_SRCFILES)
str_bench_ts_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=1 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
str_bench_ts_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_nots_lvl_tr_SOURCES = $(bench_SRCFILES)
str_bench_nots_lvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=1
str_bench_nots_lvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_nots_lvl_notr_SOURCES = $(bench_SRCFILES)
str_bench_nots_lvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_BACKTRACE=0
str_bench_nots_lvl_notr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_nots_nolvl_tr_SOURCES = $(bench_SRCFILES)
str_bench_nots_nolvl_tr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=1
str_bench_nots_nolvl_tr_LDFLAGS = $(WARN_LDFLAGS)

str_bench_nots_nolvl_notr_SOURCES = $(bench_SRCFILES)
str_bench_nots_nolvl_notr_CFLAGS = $(WARN_CFLAGS) $(test_CFLAGS) \
	-DPINELOG_SHOW_DATE=0 -DPINELOG_SHOW_LEVEL=0 -DPINELOG_SHOW_BACKTRACE=0
str_bench_nots_nolvl_notr_LDFLAGS = $(WARN_LDFLAGS)

