# Makefile to generate man & html pages from ronn input
# This is deliberately kept as a manual makefile and not as a part of
# the autotools framework for the time being.

PROJECT := x52pro-linux

MAN3_PAGES := $(wildcard *.3.ronn)
all: $(MAN3_PAGES)
	ronn --manual "Programmer's Guide" --organization="$(PROJECT)" $(MAN3_PAGES)

.PHONY: clean
clean:
	rm -f *.3
	rm -f *.html
