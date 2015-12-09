ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = x52cli

# Command line utility that front ends the core library
x52cli_SOURCES = x52_cli.c
x52cli_CFLAGS = @X52_INCLUDE@
x52cli_LDADD = @X52_CORE_LIB@

# Man pages for CLI utility
dist_man1_MANS = x52cli.man
