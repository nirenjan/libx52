ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libx52.la

# Core libx52 library
# This library handles the USB communication between the host and the X52
libx52_la_SOURCES = x52_control.c x52_core.c x52_date_time.c x52_mfd_led.c \
					x52_strerror.c
libx52_la_CFLAGS = @LIBUSB_CFLAGS@
libx52_la_LDFLAGS = -version-info 3:0:1 @LIBUSB_LIBS@

# Header files that need to be copied
x52includedir = $(includedir)/x52pro
x52include_HEADERS = libx52.h

# Extra files that need to be in the distribution
EXTRA_DIST = libx52.h x52_commands.h x52_common.h README.md
