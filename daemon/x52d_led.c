/*
 * Saitek X52 Pro MFD & LED driver - Clock manager
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"
#include "x52d_device.h"

#define SET_LED_STATE(led, state) \
    PINELOG_TRACE("Setting LED %s state to %s", \
                  libx52_str_led_id(LIBX52_LED_ ## led), \
                  libx52_str_led_state(state)); \
    x52d_dev_set_led_state(LIBX52_LED_ ## led, state);

void x52d_cfg_set_LED_Fire(libx52_led_state state)
{
    SET_LED_STATE(FIRE, state);
}

void x52d_cfg_set_LED_Throttle(libx52_led_state state)
{
    SET_LED_STATE(THROTTLE, state);
}

void x52d_cfg_set_LED_A(libx52_led_state state)
{
    SET_LED_STATE(A, state);
}

void x52d_cfg_set_LED_B(libx52_led_state state)
{
    SET_LED_STATE(B, state);
}

void x52d_cfg_set_LED_D(libx52_led_state state)
{
    SET_LED_STATE(D, state);
}

void x52d_cfg_set_LED_E(libx52_led_state state)
{
    SET_LED_STATE(E, state);
}

void x52d_cfg_set_LED_T1(libx52_led_state state)
{
    SET_LED_STATE(T1, state);
}

void x52d_cfg_set_LED_T2(libx52_led_state state)
{
    SET_LED_STATE(T2, state);
}

void x52d_cfg_set_LED_T3(libx52_led_state state)
{
    SET_LED_STATE(T3, state);
}

void x52d_cfg_set_LED_POV(libx52_led_state state)
{
    SET_LED_STATE(POV, state);
}

void x52d_cfg_set_LED_Clutch(libx52_led_state state)
{
    SET_LED_STATE(CLUTCH, state);
}

#define SET_BRIGHTNESS(mfd, brightness) \
    PINELOG_TRACE("Setting %s brightness to %u", mfd ? "MFD" : "LED", brightness); \
    x52d_dev_set_brightness(mfd, brightness);

void x52d_cfg_set_Brightness_MFD(uint16_t brightness)
{
    SET_BRIGHTNESS(1, brightness);
}

void x52d_cfg_set_Brightness_LED(uint16_t brightness)
{
    SET_BRIGHTNESS(0, brightness);
}
