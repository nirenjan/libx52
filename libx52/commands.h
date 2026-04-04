/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52_COMMANDS_H
#define X52_COMMANDS_H

/* X52 vendor API commands */
/* Vendor request - all commands must have this request ID */
#define X52_VENDOR_REQUEST      0x91

/* MFD Text commands */
#define X52_MFD_CLEAR_LINE      0x08
#define X52_MFD_WRITE_LINE      0x00

#define X52_MFD_LINE1           0xd1
#define X52_MFD_LINE2           0xd2
#define X52_MFD_LINE3           0xd4

/* Brightness commands */
#define X52_MFD_BRIGHTNESS      0xb1
#define X52_LED_BRIGHTNESS      0xb2

/* LED set commands */
#define X52_LED                 0xb8

/* Time commands */
#define X52_TIME_CLOCK1         0xc0
#define X52_OFFS_CLOCK2         0xc1
#define X52_OFFS_CLOCK3         0xc2

/* Date commands */
#define X52_DATE_DDMM           0xc4
#define X52_DATE_YEAR           0xc8

/* Shift indicator on MFD */
#define X52_SHIFT_INDICATOR     0xfd
#define X52_SHIFT_ON            0x51
#define X52_SHIFT_OFF           0x50

/* Blink throttle & POV LED */
#define X52_BLINK_INDICATOR     0xb4
#define X52_BLINK_ON            0x51
#define X52_BLINK_OFF           0x50

#endif /* !defined X52_COMMANDS_H */
