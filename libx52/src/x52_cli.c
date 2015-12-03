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
    { "off",        LIBX52_LED_STATE_OFF },
    { "on",         LIBX52_LED_STATE_ON },
    { "red",        LIBX52_LED_STATE_RED },
    { "amber",      LIBX52_LED_STATE_AMBER },
    { "green",      LIBX52_LED_STATE_GREEN },
    { NULL,         -1 }
};

/* Map for LED identifier */
DEFINE_MAP(led_id) = {
    { "fire",       LIBX52_LED_FIRE },
    { "a",          LIBX52_LED_A },
    { "b",          LIBX52_LED_B },
    { "d",          LIBX52_LED_D },
    { "e",          LIBX52_LED_E },
    { "t1",         LIBX52_LED_T1 },
    { "t2",         LIBX52_LED_T2 },
    { "t3",         LIBX52_LED_T3 },
    { "pov",        LIBX52_LED_POV },
    { "clutch",     LIBX52_LED_CLUTCH },
    { "throttle",   LIBX52_LED_THROTTLE },
    { NULL,         -1 }
};

/* Map for date format */
DEFINE_MAP(date_format) = {
    { "ddmmyy",     LIBX52_DATE_FORMAT_DDMMYY },
    { "mmddyy",     LIBX52_DATE_FORMAT_MMDDYY },
    { "yymmdd",     LIBX52_DATE_FORMAT_YYMMDD },
    { NULL,         -1 }
};

/* Map for brightness setting */
DEFINE_MAP(brightness_targets) = {
    { "mfd",        1 },
    { "led",        0 },
    { NULL,         -1 }
};

/* Map for blink/shift on/off */
DEFINE_MAP(on_off) = {
    { "off",        0  },
    { "on",         1  },
    { NULL,         -1 }
};

/* Map for clock 0 timezone */
DEFINE_MAP(clock0_timezone) = {
    { "gmt",        0  },
    { "local",      1  },
    { NULL,         -1 }
};

/* Map for identifying the clock for the timezone */
DEFINE_MAP(clocks) = {
    { "1",          LIBX52_CLOCK_1 },
    { "2",          LIBX52_CLOCK_2 },
    { "3",          LIBX52_CLOCK_3 },
    { NULL,         -1 }
};

/* Map for identifying the time format */
DEFINE_MAP(time_format) = {
    { "12hr",       LIBX52_CLOCK_FORMAT_12HR },
    { "24hr",       LIBX52_CLOCK_FORMAT_24HR },
    { NULL,         -1 }
};

/* Map for commands */
DEFINE_MAP(command) = {
    { "led",        X52_CTL_CMD_LED_STATE },
    { "bri",        X52_CTL_CMD_BRIGHTNESS },
    { "mfd",        X52_CTL_CMD_MFD_TEXT },
    { "blink",      X52_CTL_CMD_BLINK },
    { "shift",      X52_CTL_CMD_SHIFT },
    { "clock",      X52_CTL_CMD_CLOCK },
    { "offset",     X52_CTL_CMD_OFFSET },
    { NULL,         -1 }
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
};

static int do_help(const struct command_handler *cmd)
{
    int i;
    if (cmd) {
        fprintf(stderr, "Command usage: %s", cmd->help);        
    } else {
        printf("\nCommands:\n");
        for (i = 0; i < X52_CTL_CMD_MAX; i++) {
            printf("\t%s\n", handlers[i].help);
        }

        printf("\n");
    }
}

int main(int argc, char **argv)
{
    libx52_device *x52;
    int command;
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
                fprintf(stderr, "Invalid argument %s", argv[2+i]);
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
