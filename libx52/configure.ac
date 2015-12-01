AC_INIT([libx52], [1.0.0], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
AM_PROG_AR
LT_INIT

# Check for libusb-1.0
AC_SEARCH_LIBS([libusb_init], [libusb-1.0], [have_libusb=yes])

if test "x${have_libusb}" = xyes; then
    AC_CHECK_HEADERS([libusb-1.0/libusb.h], [], [have_libusb=no])
fi

if test "x${have_libusb}" = xno; then
    AC_MSG_ERROR([
    -------------------------------------------------------
     The library libusb-1.0 and header file libusb.h
     are required to build libx52. Stopping...
     Check 'config.log' for more information.
    -------------------------------------------------------
    ])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
