/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "libx52.h"

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

static int update_raw(libx52_device *x52, void *args[])
{
    uint16_t wIndex = (uint16_t)strtoul(args[0], NULL, 0);
    uint16_t wValue = (uint16_t)strtoul(args[1], NULL, 0);

    return libx52_vendor_command(x52, wIndex, wValue);
}

/* Commands for CLI */
#define COMMANDS \
    X(led,      LED_STATE,  "<led-id> <state>",     2, \
        MAP(led_id), MAP(led_state)) \
    X(bri,      BRIGHTNESS, "{mfd | led} <brightness level>", 2, \
        MAP(brightness_targets), NULL) \
    X(mfd,      MFD_TEXT,   "<line> <text in quotes>", 2, \
        NULL, NULL) \
    X(blink,    BLINK,      "{ on | off }", 1, \
        MAP(on_off)) \
    X(shift,    SHIFT,      "{ on | off }", 1, \
        MAP(on_off)) \
    X(clock,    CLOCK, \
        "{local | gmt} {12hr | 24hr} {ddmmyy | mmddyy | yymmdd}", \
        3, MAP(clock0_timezone), MAP(time_format), MAP(date_format)) \
    X(offset,   OFFSET, \
        "{2 | 3} <offset from clock 1 in minutes> {12hr | 24hr}", \
        3, MAP(clocks), NULL, MAP(time_format)) \
    X(time,     TIME, \
        "<hour> <minute> {12hr | 24hr}", 3, \
        NULL, NULL, MAP(time_format)) \
    X(date,     DATE, \
        "<dd> <mm> <yy> {ddmmyy | mmddyy | yymmdd}", 4, \
        NULL, NULL, NULL, MAP(date_format)) \
    X(raw,      RAW,        "<wIndex> <wValue>", 2, NULL, NULL)

/* Enums for command identification */
#define X(cmd, en, help, args, ...) X52_CTL_CMD_ ## en,
enum {
    COMMANDS

    X52_CTL_CMD_MAX
};
#undef X

/* Map for commands */
#define X(cmd, en, help, args, ...) MAP_INT( #cmd, X52_CTL_CMD_ ## en),
DEFINE_MAP(command) = {
    COMMANDS

    MAP_TERMINATOR
};
#undef X

/* Command handlers */
#define X(cmd, en, help, args, ...) \
    [X52_CTL_CMD_ ## en] = { \
        update_ ## cmd, \
        args, \
        { __VA_ARGS__ }, \
        #cmd " " help \
    },
const struct command_handler handlers[X52_CTL_CMD_MAX] = {
    COMMANDS
};
#undef X

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
    rc = libx52_init(&x52);

    if (rc != LIBX52_SUCCESS) {
        fprintf(stderr, "Error initializing X52 joystick: %s\n", libx52_strerror(rc));
        return 1;
    }

    rc = (*(cmd->handler))(x52, args);
    if (rc != LIBX52_SUCCESS) {
        fprintf(stderr, "Error: %s\n", libx52_strerror(rc));
    }

    libx52_update(x52);

    libx52_exit(x52);

    return rc;
}
