/*
 * Saitek X52 Pro MFD & LED driver - Device manager header
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_DEVICE_H
#define X52D_DEVICE_H

#include "libx52.h"

void x52d_dev_init(void);
void x52d_dev_exit(void);

/* Wrapper methods for libx52 calls */
int x52d_dev_set_text(uint8_t line, const char *text, uint8_t length);
int x52d_dev_set_led_state(libx52_led_id led, libx52_led_state state);
int x52d_dev_set_clock(time_t time, int local);
int x52d_dev_set_clock_timezone(libx52_clock_id clock, int offset);
int x52d_dev_set_clock_format(libx52_clock_id clock, libx52_clock_format format);
int x52d_dev_set_time(uint8_t hour, uint8_t minute);
int x52d_dev_set_date(uint8_t dd, uint8_t mm, uint8_t yy);
int x52d_dev_set_date_format(libx52_date_format format);
int x52d_dev_set_brightness(uint8_t mfd, uint16_t brightness);
int x52d_dev_set_shift(uint8_t state);
int x52d_dev_set_blink(uint8_t state);
int x52d_dev_update(void);

#endif // !defined X52D_DEVICE_H
