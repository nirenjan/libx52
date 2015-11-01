/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#ifndef LIBX52_H
#define LIBX52_H

#include <time.h>
#include <stdint.h>

struct libx52_device;
typedef struct libx52_device libx52_device;

/**
 * @brief Initialize the X52 library
 *
 * Sets up any internal data structures to access the joystick, all
 * calls to libx52 use the returned pointer to control the device.
 *
 * @returns a pointer to the detected \ref libx52_device
 */
libx52_device * libx52_init(void);

/**
 * @brief Exit the library and free up any resources used
 *
 * @param[in]   dev     Pointer to the device 
 * @returns None
 */
void libx52_exit(libx52_device *dev);

int libx52_control_transfer(libx52_device *x52, uint16_t index, uint16_t value);
int libx52_set_text(libx52_device *x52, uint8_t line, const char *text, uint8_t length);
int libx52_set_led(libx52_device *x52, uint8_t led, uint8_t state);
int libx52_set_date(libx52_device *x52, uint8_t date, uint8_t month, uint8_t year, uint8_t format);
int libx52_set_time(libx52_device *x52, uint8_t hour, uint8_t minute, uint8_t format);
int libx52_set_clock(libx52_device *x52, time_t time);
int libx52_set_brightness(libx52_device *x52, uint8_t mfd, uint16_t brightness);
int libx52_set_shift(libx52_device *x52, uint8_t state);
int libx52_set_blink(libx52_device *x52, uint8_t state);
int libx52_update(libx52_device *x52);

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

#endif /* !defined LIBX52_H */
