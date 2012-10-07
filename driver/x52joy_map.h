/*
 * Saitek X52 Pro HOTAS driver
 *
 * Copyright (C) 2012 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#ifndef X52JOY_MAP_H
#define X52JOY_MAP_H

#include <linux/usb.h>

/**********************************************************************
 * X52 enumerations
 *********************************************************************/
enum x52_button {
    x52_button_trigger1,            /* Trigger primary */
    x52_button_launch,              /* Launch/Fire */
    x52_button_a,
    x52_button_b,
    x52_button_c,
    x52_button_pinkie,              /* Pinkie shift switch */

    x52_button_d,                   /* D and E buttons are */
    x52_button_e,                   /* on the throttle */

    x52_button_t1_up,               /* Toggle 1 up/down */
    x52_button_t1_dn,
    x52_button_t2_up,               /* Toggle 2 up/down */
    x52_button_t2_dn,
    x52_button_t3_up,               /* Toggle 3 up/down */
    x52_button_t3_dn,
    x52_button_trigger2,            /* Trigger secondary */

    x52_button_mouse_left,
    x52_button_mouse_scroll_dn,     // Down & up are from the POV of
    x52_button_mouse_scroll_up,     // the user
    x52_button_mouse_right,

    x52_button_stick_pov_N,         // These two POVs should really
    x52_button_stick_pov_E,         // be represented by hats, but
    x52_button_stick_pov_S,         // the joystick reports them as
    x52_button_stick_pov_W,         // button presses.

    x52_button_throttle_pov_N,      // For the throttle, N, E, S, W
    x52_button_throttle_pov_E,      // are taken if you look at it
    x52_button_throttle_pov_S,      // from the position of the user
    x52_button_throttle_pov_W,      // NOT from the rear of the throttle

    x52_button_mode_select_1,       // The mode selection switch is not
    x52_button_mode_select_2,       // a push button but a selector - 
    x52_button_mode_select_3,       // only one can be active at a time

    x52_button_clutch,
    x52_button_function,
    x52_button_start_stop,
    x52_button_reset,
    x52_button_pg_up,
    x52_button_pg_dn,
    x52_button_up,
    x52_button_dn,
    x52_button_select,

    x52_button_max
};

enum x52_axis {
    x52_axis_x,             /* Stick X axis */
    x52_axis_y,             /* Stick Y axis */
    x52_axis_rz,            /* Stick twist axis */

    x52_axis_z,             /* Throttle axis */
    x52_axis_rx,            /* Rotary X axis */
    x52_axis_ry,            /* Rotary Y axis */
    x52_axis_slider,        /* Slider axis */

    x52_axis_mouse_x,       /* Mouse stick X axis */
    x52_axis_mouse_y,       /* Mouse stick Y axis */

    x52_axis_max
};

/**********************************************************************
 * X52 joystick settings
 *********************************************************************/
/* Deadzone settings - no response if value ... */ 
struct x52_axis_deadzone {
    u16     dead_lo;        /* value < dead_lo */
    u16     dead_hi;        /* value > dead_hi */

    u16     dead_mid_lo;    /* dead_mid_lo < dead_mid_hi && */
    u16     dead_mid_hi;    /* dead_mid_lo < value < dead_mid_hi */
};

/*
 * The X52 MFD supports the following:
 *  - 3 lines of 16 characters each
 *  - Clock with HH:MM
 *  - Date with YYMMDD (IIRC)
 */
#define X52_MFD_LINE_SIZE   16
#define X52_MFD_LINES       3

struct x52_mfd_line {
    u8      text[X52_MFD_LINE_SIZE];
    u8      length;
};

enum x52_mfd_date_format {
    x52_mfd_format_yymmdd,  /* YY-MM-DD */
    x52_mfd_format_mmddyy,  /* MM-DD-YY */
    x52_mfd_format_ddmmyy,  /* DD-MM-YY */
    x52_mfd_format_max,
};

struct x52_mfd_date {
    u8      year;
    u8      month;
    u8      day;
    u8      format;     /* See format enum */
};

struct x52_mfd_time {
    u8      hour;
    u8      minute;
    u8      h24;        /* 24 hour format if 1 */
};

struct x52_mfd_offs {
    u8      min_off;    /* Minute offset from clock 0 */
    u8      neg_off;    /* Negative offset if 1 */
    u8      h24;        /* 24 hour format if 1 */
};

/**********************************************************************
 * X52 mappings
 *********************************************************************/
/*
 * Modes to map
 *
 * Right now, the module supports only 6 modes, listed as follows:
 * - Mode 1
 * - Mode 2
 * - Mode 3
 * - Mode 1 + Pinkie
 * - Mode 2 + Pinkie
 * - Mode 3 + Pinkie
 * The last 3 modes use the pinkie switch as a shift selector.
 */
enum x52_map_modes {
    x52_map_mode1,
    x52_map_mode2,
    x52_map_mode3,
    x52_map_mode1shift,
    x52_map_mode2shift,
    x52_map_mode3shift,
    x52_map_max
};

/* Structure to map joystick button input to a keyboard or mouse event */
struct x52_event {
    u16     type;
    u16     code;
    s32     value;
};

/*
 * This defines the maximum number of keyboard or mouse events that can
 * be programmed.
 */
#define X52_MAP_MAX_EVENTS  64

struct x52_map_event {
    u8      mode_selector;
    u8      flags;
    u8      button_or_axis;
    u8      fallback;

    u16     range_low;
    u16     range_high;

    struct x52_event    events[X52_MAP_MAX_EVENTS];
};

#endif /* !defined X52JOY_MAP_H */
