/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "libx52.h"

typedef enum {
    X52_CTL_CMD_BRIGHTNESS,
    X52_CTL_CMD_MFD_TEXT,
    X52_CTL_CMD_LED_STATE,
    X52_CTL_CMD_BLINK,
    X52_CTL_CMD_SHIFT,
    X52_CTL_CMD_CLOCK,
    X52_CTL_CMD_OFFSET,
    X52_CTL_CMD_TIME,
    X52_CTL_CMD_DATE,
    X52_CTL_CMD_RAW,

    X52_CTL_CMD_MAX
} x52_ctl_command;

struct string_map {
    char *key;
    union {
        libx52_clock_id     clock_id;
        libx52_clock_format clock_format;
        libx52_date_format  date_format;
        libx52_led_id       led_id;
        libx52_led_state    led_state;
        int                 int_val;
        intptr_t            ptr_val;
    } value;
};

// Max 4 arguments for now
#define MAX_ARGS    4

typedef int (*handler_cb)(libx52_device *x52, void *args[]);
struct command_handler {
    handler_cb handler;
    int num_args;
    const struct string_map *maps[MAX_ARGS];
    const char *help;
};

#define SAVE_ARGUMENT(type, name, arg) type name = (type)(uintptr_t)(arg)
#define PARSE_ARGUMENT(type, arg) ((type)(uintptr_t)(arg))

#define MAP(name)   name##_map
#define DEFINE_MAP(name) static const struct string_map MAP(name)[]
#define MAP_CLOCK_ID(id)        {.key = #id, .value.clock_id = LIBX52_CLOCK_ ## id}
#define MAP_CLOCK_FORMAT(fmt)   {.key = #fmt, .value.clock_format = LIBX52_CLOCK_FORMAT_ ## fmt}
#define MAP_DATE_FORMAT(fmt)    {.key = #fmt, .value.date_format = LIBX52_DATE_FORMAT_ ## fmt}
#define MAP_LED_ID(id)          {.key = #id, .value.led_id = LIBX52_LED_ ## id}
#define MAP_LED_STATE(state)    {.key = #state, .value.led_state = LIBX52_LED_STATE_ ## state}
#define MAP_INT(str, val)       {.key = str, .value.int_val = val}
#define MAP_TERMINATOR  MAP_INT(NULL, -1)

/**
 * Parse a string and match it with a corresponding value
 * Maps are arrays which contain a terminating entry with a NULL key.
 *
 * Return 0 if no match
 */
static int map_lookup(const struct string_map *map, const char *str, struct string_map *result)
{
    const struct string_map *map_index = map;

    /* Search through the map for a matching key */
    while (map_index->key) {
        if (!strcasecmp(str, map_index->key)) {
            result->value = map_index->value;
            return 1;
        }

        map_index++;
    }

    return 0;
}

/* Map for LED state */
DEFINE_MAP(led_state) = {
    MAP_LED_STATE(OFF),
    MAP_LED_STATE(ON),
    MAP_LED_STATE(RED),
    MAP_LED_STATE(AMBER),
    MAP_LED_STATE(GREEN),
    MAP_TERMINATOR
};

/* Map for LED identifier */
DEFINE_MAP(led_id) = {
    MAP_LED_ID(FIRE),
    MAP_LED_ID(A),
    MAP_LED_ID(B),
    MAP_LED_ID(D),
    MAP_LED_ID(E),
    MAP_LED_ID(T1),
    MAP_LED_ID(T2),
    MAP_LED_ID(T3),
    MAP_LED_ID(POV),
    MAP_LED_ID(CLUTCH),
    MAP_LED_ID(THROTTLE),
    MAP_TERMINATOR
};

/* Map for date format */
DEFINE_MAP(date_format) = {
    MAP_DATE_FORMAT(DDMMYY),
    MAP_DATE_FORMAT(MMDDYY),
    MAP_DATE_FORMAT(YYMMDD),
    MAP_TERMINATOR
};

/* Map for brightness setting */
DEFINE_MAP(brightness_targets) = {
    MAP_INT( "mfd", 1 ),
    MAP_INT( "led", 0 ),
    MAP_TERMINATOR
};

/* Map for blink/shift on/off */
DEFINE_MAP(on_off) = {
    MAP_INT( "off", 0  ),
    MAP_INT( "on",  1  ),
    MAP_TERMINATOR
};

/* Map for clock 0 timezone */
DEFINE_MAP(clock0_timezone) = {
    MAP_INT( "gmt",     0  ),
    MAP_INT( "local",   1  ),
    MAP_TERMINATOR
};

/* Map for identifying the clock for the timezone */
DEFINE_MAP(clocks) = {
    MAP_CLOCK_ID(1),
    MAP_CLOCK_ID(2),
    MAP_CLOCK_ID(3),
    MAP_TERMINATOR
};

/* Map for identifying the time format */
DEFINE_MAP(time_format) = {
    MAP_CLOCK_FORMAT(12HR),
    MAP_CLOCK_FORMAT(24HR),
    MAP_TERMINATOR
};

/* Map for commands */
DEFINE_MAP(command) = {
    MAP_INT( "led",     X52_CTL_CMD_LED_STATE ),
    MAP_INT( "bri",     X52_CTL_CMD_BRIGHTNESS ),
    MAP_INT( "mfd",     X52_CTL_CMD_MFD_TEXT ),
    MAP_INT( "blink",   X52_CTL_CMD_BLINK ),
    MAP_INT( "shift",   X52_CTL_CMD_SHIFT ),
    MAP_INT( "clock",   X52_CTL_CMD_CLOCK ),
    MAP_INT( "offset",  X52_CTL_CMD_OFFSET ),
    MAP_INT( "time",    X52_CTL_CMD_TIME ),
    MAP_INT( "date",    X52_CTL_CMD_DATE ),
    MAP_INT( "raw",     X52_CTL_CMD_RAW ),
    MAP_TERMINATOR
};

static int update_led(libx52_device *x52, void *args[])
{
    return libx52_set_led_state(x52,
        PARSE_ARGUMENT(libx52_led_id, args[0]),
        PARSE_ARGUMENT(libx52_led_state, args[1]));
}

static int update_bri(libx52_device *x52, void *args[])
{
    unsigned long int brightness = strtoul(args[1], NULL, 0);

    return libx52_set_brightness(x52,
        PARSE_ARGUMENT(uint8_t, args[0]), (uint16_t)brightness);
}

static int update_mfd(libx52_device *x52, void *args[])
{
    uint8_t line = (uint8_t)strtoul(args[0], NULL, 0);
    uint8_t length = strlen(args[1]);

    return libx52_set_text(x52, line, args[1], length);
}

static int update_blink(libx52_device *x52, void *args[])
{
    return libx52_set_blink(x52, PARSE_ARGUMENT(int, args[0]));
}

static int update_shift(libx52_device *x52, void *args[])
{
    return libx52_set_shift(x52, PARSE_ARGUMENT(int, args[0]));
}

static int update_clock(libx52_device *x52, void *args[])
{
    int rc;
    rc = libx52_set_clock(x52, time(NULL),
        PARSE_ARGUMENT(int, args[0]));

    if (!rc) {
        rc = libx52_set_clock_format(x52, LIBX52_CLOCK_1,
                PARSE_ARGUMENT(libx52_clock_format, args[1]));
    }

    if (!rc) {
        rc = libx52_set_date_format(x52,
            PARSE_ARGUMENT(libx52_date_format, args[2]));
    }

    return rc;
}

static int update_offset(libx52_device *x52, void *args[])
{
    int offset = (int)strtol(args[1], NULL, 0);
    int rc;
    SAVE_ARGUMENT(libx52_clock_id, clock, args[0]);

    rc = libx52_set_clock_timezone(x52, clock, offset);

    if (!rc) {
        rc = libx52_set_clock_format(x52, clock,
            PARSE_ARGUMENT(libx52_clock_format, args[2]));
    }

    return rc;
}

static int update_time(libx52_device *x52, void *args[])
{
    int hh = (int)strtol(args[0], NULL, 0);
    int mm = (int)strtol(args[1], NULL, 0);
    int rc;

    /* Set the time value */
    rc = libx52_set_time(x52, hh, mm);
    if (!rc) {
        rc = libx52_set_clock_format(x52, LIBX52_CLOCK_1,
            PARSE_ARGUMENT(libx52_clock_format, args[2]));
    }

    return rc;
}

static int update_date(libx52_device *x52, void *args[])
{
    int dd = (int)strtol(args[0], NULL, 0);
    int mm = (int)strtol(args[1], NULL, 0);
    int yy = (int)strtol(args[2], NULL, 0);
    int rc;

    /* Set the date value */
    rc = libx52_set_date(x52, dd, mm, yy);
    if (!rc) {
        rc = libx52_set_date_format(x52,
            PARSE_ARGUMENT(libx52_date_format, args[3]));
    }

    return rc;
}

static int write_raw(libx52_device *x52, void *args[])
{
    uint16_t wIndex = (uint16_t)strtoul(args[0], NULL, 0);
    uint16_t wValue = (uint16_t)strtoul(args[1], NULL, 0);

    return libx52_vendor_command(x52, wIndex, wValue);
}

const struct command_handler handlers[X52_CTL_CMD_MAX] = {
    [X52_CTL_CMD_LED_STATE] = {
        update_led,
        2,
        {
            MAP(led_id),
            MAP(led_state),
        },
        "led <led-id> <state>"
    },
    [X52_CTL_CMD_BRIGHTNESS] = {
        update_bri,
        2,
        {
            MAP(brightness_targets),
            NULL,
        },
        "bri {mfd | led} <brightness level>"
    },
    [X52_CTL_CMD_MFD_TEXT] = {
        update_mfd,
        2,
        {
            NULL,
            NULL,
        },
        "mfd <line> <text in quotes>"
    },
    [X52_CTL_CMD_BLINK] = {
        update_blink,
        1,
        { MAP(on_off) },
        "blink { on | off }"
    },
    [X52_CTL_CMD_SHIFT] = {
        update_shift,
        1,
        { MAP(on_off) },
        "shift { on | off }"
    },
    [X52_CTL_CMD_CLOCK] = {
        update_clock,
        3,
        {
            MAP(clock0_timezone),
            MAP(time_format),
            MAP(date_format),
        },
        "clock {local | gmt} {12hr | 24hr} {ddmmyy | mmddyy | yymmdd}"
    },
    [X52_CTL_CMD_OFFSET] = {
        update_offset,
        3,
        {
            MAP(clocks),
            NULL,
            MAP(time_format)
        },
        "offset {2 | 3} <offset from clock 1 in minutes> {12hr | 24hr}"
    },
    [X52_CTL_CMD_TIME] = {
        update_time,
        3,
        {
            NULL,
            NULL,
            MAP(time_format)
        },
        "time <hour> <minute> {12hr | 24hr}"
    },
    [X52_CTL_CMD_DATE] = {
        update_date,
        4,
        {
            NULL,
            NULL,
            NULL,
            MAP(date_format)
        },
        "date <dd> <mm> <yy> {ddmmyy | mmddyy | yymmdd}"
    },
    [X52_CTL_CMD_RAW] = {
        write_raw,
        2,
        {
            NULL,
            NULL,
        },
        "raw <wIndex> <wValue>"
    }
};

static void do_help(const struct command_handler *cmd)
{
    int i;
    if (cmd) {
        fprintf(stderr, "Command usage: %s\n", cmd->help);
    } else {
        printf("\nCommands:\n");
        for (i = 0; i < X52_CTL_CMD_MAX; i++) {
            printf("\t%s\n", handlers[i].help);
        }

        printf("\nWARNING: raw command may damage your device\n\n");
    }
}

int main(int argc, char **argv)
{
    libx52_device *x52;
    struct string_map result;
    const struct command_handler *cmd;
    int i;
    void *args[MAX_ARGS];
    int rc;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [arguments]\n", argv[0]);
        do_help(NULL);
        return 1;
    }

    if (!map_lookup(command_map, argv[1], &result)) {
        fprintf(stderr, "Unsupported command %s\n", argv[1]);
        do_help(NULL);
        return 1;
    }

    cmd = &handlers[result.value.int_val];
    if (!cmd->handler) {
        fprintf(stderr, "Command %s not implemented yet!\n", argv[1]);
        return 1;
    }

    if (cmd->num_args > argc - 2) {
        fprintf(stderr, "Insufficient arguments for command %s\n", argv[1]);
        do_help(cmd);
        return 1;
    }

    /* Clear the arguments array */
    memset(args, 0, sizeof(args));

    for (i = 0; i < cmd->num_args; i++) {
        if (cmd->maps[i]) {
            if (!map_lookup(cmd->maps[i], argv[2+i], &result)) {
                fprintf(stderr, "Invalid argument %s\n", argv[2+i]);
                return 1;
            }
            args[i] = (void *)result.value.ptr_val;
        } else {
            args[i] = argv[2+i];
        }
    }

    /* Lookup the X52 device */
    x52 = libx52_init();

    if (!x52) {
        fprintf(stderr, "Unable to find X52 joystick!\n");
        return 1;
    }

    rc = (*(cmd->handler))(x52, args);
    if (rc != 0) {
        fprintf(stderr, "Error: %s\n", strerror(-rc));
    }

    libx52_update(x52);

    libx52_exit(x52);

    return rc;
}
