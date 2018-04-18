/*
 * Saitek X52 Pro HOTAS driver
 *
 * Copyright (C) 2012 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef X52JOY_COMMANDS_H
#define X52JOY_COMMANDS_H

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

/* LED indices */
#define X52_LED_FIRE            0x01
#define X52_LED_A_RED           0x02
#define X52_LED_A_GREEN         0x03
#define X52_LED_B_RED           0x04
#define X52_LED_B_GREEN         0x05
#define X52_LED_D_RED           0x06
#define X52_LED_D_GREEN         0x07
#define X52_LED_E_RED           0x08
#define X52_LED_E_GREEN         0x09
#define X52_LED_T1_RED          0x0a
#define X52_LED_T1_GREEN        0x0b
#define X52_LED_T2_RED          0x0c
#define X52_LED_T2_GREEN        0x0d
#define X52_LED_T3_RED          0x0e
#define X52_LED_T3_GREEN        0x0f
#define X52_LED_POV_RED         0x10
#define X52_LED_POV_GREEN       0x11
#define X52_LED_I_RED           0x12
#define X52_LED_I_GREEN         0x13
#define X52_LED_THROTTLE        0x14

#endif /* !defined X52JOY_COMMANDS_H */
