ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libx52.la
libx52_la_SOURCES = src/x52_common.h src/x52_commands.h src/libx52.h \
					src/x52_control.c src/x52_core.c \
					src/x52_date_time.c src/x52_mfd_led.c
libx52_la_LDFLAGS = -version-info 1:0:0 -lusb-1.0
pkginclude_HEADERS = src/libx52.h

noinst_PROGRAMS = x52_test
x52_test_SOURCES = src/x52_test.c
x52_test_LDADD = libx52.la

