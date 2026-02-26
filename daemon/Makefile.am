# Automake for x52d
#
# Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
bin_PROGRAMS += x52d x52ctl

# Service daemon that manages the X52 device
x52d_SOURCES = \
	daemon/x52d_main.c \
	daemon/x52d_config_parser.c \
	daemon/x52d_config_dump.c \
	daemon/x52d_config.c \
	daemon/x52d_device.c \
	daemon/x52d_client.c \
	daemon/x52d_clock.c \
	daemon/x52d_mouse.c \
	daemon/x52d_notify.c \
	daemon/x52d_led.c \
	daemon/x52d_command.c \
	daemon/x52d_comm_internal.c \
	daemon/x52d_comm_client.c

x52d_CFLAGS = \
	-I $(top_srcdir) \
	-I $(top_srcdir)/libx52io \
	-I $(top_srcdir)/libx52 \
	-I $(top_srcdir)/libx52util \
	-I $(top_srcdir)/lib/pinelog \
	-I $(top_srcdir)/lib/inih \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLOGDIR=\"$(localstatedir)/log\" \
	-DRUNDIR=\"$(localstatedir)/run\" \
	@PTHREAD_CFLAGS@ $(WARN_CFLAGS)

x52d_LDFLAGS = @PTHREAD_LIBS@ $(WARN_LDFLAGS)
x52d_LDADD = \
	lib/pinelog/libpinelog.la \
	lib/inih/libinih.la \
	libx52.la \
	@LTLIBINTL@

if HAVE_EVDEV
x52d_SOURCES += \
	daemon/x52d_io.c \
	daemon/x52d_mouse_evdev.c \
	daemon/x52d_keyboard_evdev.c \
	daemon/x52d_profile.c \
	daemon/x52d_clutch.c

x52d_CFLAGS += -DHAVE_EVDEV @EVDEV_CFLAGS@
x52d_LDFLAGS += @EVDEV_LIBS@
x52d_LDADD += libx52io.la
endif

lib_LTLIBRARIES += libx52dcomm.la

# Client library to communicate with X52 daemon
libx52dcomm_la_SOURCES = \
	daemon/x52d_comm_client.c \
	daemon/x52d_comm_internal.c
libx52dcomm_la_CFLAGS = \
	-I $(top_srcdir) \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLOGDIR=\"$(localstatedir)/log\" \
	-DRUNDIR=\"$(localstatedir)/run\" \
	$(WARN_CFLAGS)
libx52dcomm_la_LDFLAGS = $(WARN_LDFLAGS)

x52include_HEADERS += daemon/x52dcomm.h

x52ctl_SOURCES = daemon/x52ctl.c
x52ctl_CFLAGS = \
	-I $(top_srcdir) \
	$(WARN_CFLAGS)
x52ctl_LDFLAGS = $(WARN_LDFLAGS)
x52ctl_LDADD = libx52dcomm.la @LTLIBINTL@

x52dconfdir = @sysconfdir@/x52d
x52dconf_DATA = daemon/x52d.conf

install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(localstatedir)/log
	$(MKDIR_P) $(DESTDIR)$(localstatedir)/run

EXTRA_DIST += \
	daemon/daemon.dox \
	daemon/default.conf.example \
	daemon/protocol.dox \
	daemon/x52d.service.in \
	daemon/x52d_client.h \
	daemon/x52d_clock.h \
	daemon/x52d_config.def \
	daemon/x52d_config.h \
	daemon/x52d_const.h \
	daemon/x52d_device.h \
	daemon/x52d_io.h \
	daemon/x52d_clutch.h \
	daemon/x52d_keyboard.h \
	daemon/x52d_mouse.h \
	daemon/x52d_profile.h \
	daemon/x52d_notify.h \
	daemon/x52d_command.h \
	daemon/x52dcomm.h \
	daemon/x52dcomm-internal.h \
	daemon/x52d.conf

# Test cases
EXTRA_DIST += \
	daemon/test_daemon_comm.py \
	daemon/x52d_config_apply_stubs.c \
	daemon/x52d_profile_test.c \
	daemon/x52d_clutch_test.c \
	daemon/tests/config/args.tc \
	daemon/tests/config/clock.tc \
	daemon/tests/config/led.tc \
	daemon/tests/config/mouse.tc \
	daemon/tests/logging/error.tc \
	daemon/tests/logging/global.tc \
	daemon/tests/logging/module.tc \
	daemon/tests/cli.tc

TESTS += daemon/test_daemon_comm.py

if HAVE_CMOCKA
check_PROGRAMS += x52d-mouse-test

x52d_mouse_test_SOURCES = \
	daemon/x52d_mouse_test.c \
	daemon/x52d_mouse.c
x52d_mouse_test_CFLAGS = \
	-DLOCALEDIR='"$(localedir)"' \
	-I $(top_srcdir) \
	-I $(top_srcdir)/libx52 \
	-I $(top_srcdir)/libx52io \
	-I $(top_srcdir)/lib/pinelog \
	$(WARN_CFLAGS) @CMOCKA_CFLAGS@
x52d_mouse_test_LDFLAGS = @CMOCKA_LIBS@ $(WARN_LDFLAGS)
x52d_mouse_test_LDADD = \
	lib/pinelog/libpinelog.la \
	@LTLIBINTL@

TESTS += x52d-mouse-test
endif

if HAVE_CMOCKA
if HAVE_EVDEV
check_PROGRAMS += x52d-profile-test x52d-clutch-test

x52d_profile_test_SOURCES = \
	daemon/x52d_profile_test.c \
	daemon/x52d_profile.c \
	daemon/x52d_config.c \
	daemon/x52d_config_parser.c \
	daemon/x52d_config_dump.c \
	daemon/x52d_config_apply_stubs.c
x52d_profile_test_CFLAGS = \
	-DHAVE_EVDEV \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLOGDIR=\"$(localstatedir)/log\" \
	-DRUNDIR=\"$(localstatedir)/run\" \
	-I $(top_srcdir) \
	-I $(top_srcdir)/daemon \
	-I $(top_srcdir)/libx52 \
	-I $(top_srcdir)/libx52io \
	-I $(top_srcdir)/libx52util \
	-I $(top_srcdir)/lib/pinelog \
	-I $(top_srcdir)/lib/inih \
	$(WARN_CFLAGS) @CMOCKA_CFLAGS@ @EVDEV_CFLAGS@
x52d_profile_test_LDFLAGS = $(WARN_LDFLAGS)
x52d_profile_test_LDADD = \
	lib/pinelog/libpinelog.la \
	lib/inih/libinih.la \
	libx52io.la \
	libx52.la \
	@EVDEV_LIBS@ \
	@CMOCKA_LIBS@ \
	@LTLIBINTL@

x52d_clutch_test_SOURCES = \
	daemon/x52d_clutch_test.c \
	daemon/x52d_clutch.c
x52d_clutch_test_CFLAGS = \
	-DHAVE_EVDEV \
	-DLOCALEDIR=\"$(localedir)\" \
	-I $(top_srcdir) \
	-I $(top_srcdir)/daemon \
	-I $(top_srcdir)/libx52 \
	-I $(top_srcdir)/libx52io \
	-I $(top_srcdir)/lib/pinelog \
	$(WARN_CFLAGS) @CMOCKA_CFLAGS@
x52d_clutch_test_LDFLAGS = $(WARN_LDFLAGS)
x52d_clutch_test_LDADD = \
	lib/pinelog/libpinelog.la \
	libx52.la \
	libx52io.la \
	@CMOCKA_LIBS@ \
	@LTLIBINTL@

TESTS += x52d-profile-test x52d-clutch-test
endif
endif

if HAVE_SYSTEMD
if !IS_MAKE_DISTCHECK
SED_ARGS = s,%bindir%,$(bindir),g
x52d.service: daemon/x52d.service.in
	$(AM_V_GEN) $(SED) -e '$(SED_ARGS)' $< > $@
systemdsystemunit_DATA = x52d.service
endif
endif
