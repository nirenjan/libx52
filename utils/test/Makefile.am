ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = x52test

# Test utility that exercises all the library functions
x52test_SOURCES = x52_test.c x52_test_mfd.c x52_test_led.c x52_test_clock.c
x52test_CFLAGS = @X52_INCLUDE@
x52test_LDADD = ../../lib/libx52/libx52.la

# Extra files that need to be in the distribution
EXTRA_DIST = x52_test_common.h
