ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libx52.la
libx52_la_SOURCES = src/x52_common.h src/x52_commands.h src/libx52.h \
					src/x52_control.c src/x52_core.c \
					src/x52_date_time.c src/x52_mfd_led.c
libx52_la_LDFLAGS = -version-info 2:0:1 -lusb-1.0
pkginclude_HEADERS = src/libx52.h

bin_PROGRAMS = x52cli x52test
x52cli_SOURCES = src/x52_cli.c
x52cli_LDADD = libx52.la

x52test_SOURCES = src/x52_test.c src/x52_test_mfd.c src/x52_test_led.c \
				  src/x52_test_clock.c src/x52_test_common.h
x52test_LDADD = libx52.la
