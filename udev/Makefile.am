# udev rules installation
#
# Copyright (C) 2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

# Extra files that need to be in the distribution
EXTRA_DIST = 60-saitek-x52-x52pro.rules

if HAVE_UDEV

# udev rules need to go to (typically) /lib/udev/rules.d. This location is only
# writable by root, and if we're running make distcheck, we're most likely not
# running as root. Therefore, disable this if we're running make distcheck
if !IS_MAKE_DISTCHECK
udevrules_DATA = 60-saitek-x52-x52pro.rules

# Update udev only if being installed by root
install-data-hook:
	$(AM_V_at)if [ "$$(id -u)" = "0" ]; then \
		echo "Installing udev rules" && \
		udevadm control --reload-rules && \
		udevadm trigger --subsystem-match=usb --attr-match=idVendor=06a3 --action=add \
	;fi
endif

endif
