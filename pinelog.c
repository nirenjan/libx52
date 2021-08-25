/*
 * Pinelog lightweight logging library
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "pinelog.h"

/**********************************************************************
 * Configure defaults
 *********************************************************************/
#ifndef PINELOG_DEFAULT_STREAM
#define PINELOG_DEFAULT_STREAM stdout
#endif

#ifndef PINELOG_DEFAULT_LEVEL
#define PINELOG_DEFAULT_LEVEL PINELOG_LVL_ERROR
#endif

/**********************************************************************
 * Configure logging parameters
 *********************************************************************/
#ifndef PINELOG_SHOW_DATE
#define PINELOG_SHOW_DATE 0
#endif

#ifndef PINELOG_SHOW_LEVEL
#define PINELOG_SHOW_LEVEL 0
#endif

#ifndef PINELOG_SHOW_BACKTRACE
#define PINELOG_SHOW_BACKTRACE 0
#endif

/**********************************************************************
 * Configure level strings
 *********************************************************************/
#ifndef PINELOG_FATAL_STR
#define PINELOG_FATAL_STR "FATAL"
#endif

#ifndef PINELOG_ERROR_STR
#define PINELOG_ERROR_STR "ERROR"
#endif

#ifndef PINELOG_WARNING_STR
#define PINELOG_WARNING_STR "WARNING"
#endif

#ifndef PINELOG_INFO_STR
#define PINELOG_INFO_STR "INFO"
#endif

#ifndef PINELOG_DEBUG_STR
#define PINELOG_DEBUG_STR "DEBUG"
#endif

#ifndef PINELOG_TRACE_STR
#define PINELOG_TRACE_STR "TRACE"
#endif

/**********************************************************************
 * Global variables
 *********************************************************************/

/** Stream buffer */
static FILE *output_stream = NULL;

/** Default logging level */
static int log_level = PINELOG_DEFAULT_LEVEL;

/* Initialize defaults */
#if defined __has_attribute
#   if __has_attribute(constructor)
        __attribute__((constructor))
#   endif
#endif
void pinelog_set_defaults(void)
{
    output_stream = PINELOG_DEFAULT_STREAM;
    log_level = PINELOG_DEFAULT_LEVEL;
}

#if defined __has_attribute
#   if __has_attribute(destructor)
        __attribute__((destructor))
#   endif
#endif
void pinelog_close_output_stream(void)
{
    /* If current output stream is not stdout or stderr, then close it */
    if (output_stream != NULL && output_stream != stdout && output_stream != stderr) {
        fclose(output_stream);
    }
    output_stream = PINELOG_DEFAULT_STREAM;
}

int pinelog_set_output_stream(FILE *stream)
{
    if (stream == NULL) {
        return EINVAL;
    }

    pinelog_close_output_stream();

    setlinebuf(stream);
    output_stream = stream;
    return 0;
}

#ifdef PINELOG_TEST
FILE * pinelog_get_output_stream(void)
{
    return output_stream;
}
#endif

int pinelog_set_output_file(const char *file)
{
    FILE *stream;
    if (file == NULL) {
        return EINVAL;
    }

    errno = 0;
    stream = fopen(file, "w");
    if (stream == NULL) {
        return errno;
    }

    return pinelog_set_output_stream(stream);
}

int pinelog_get_level(void)
{
    return log_level;
}

int pinelog_set_level(int level)
{
    if (level < PINELOG_LVL_NONE || level > PINELOG_LVL_TRACE) {
        return EINVAL;
    }

    log_level = level;
    return 0;
}

/**********************************************************************
 * Log the message to the output stream
 *********************************************************************/
void pinelog_log_message(int level, const char *file, int line, const char *fmt, ...)
{
    va_list ap;

    /* Don't log anything if the level is not severe enough */
    if (level > log_level || level < 0) {
        return;
    }

    /* Cap the log level */
    if (level > PINELOG_LVL_TRACE) {
        level = PINELOG_LVL_TRACE;
    }

    #if defined __has_attribute
    #if !__has_attribute(constructor)
    /*
     * Validate and set output stream. Only necessary if the compiler doesn't
     * support the constructor attribute
     */
    if (output_stream == NULL) {
        output_stream = PINELOG_DEFAULT_STREAM;
    }
    #endif
    #endif

    #if PINELOG_SHOW_DATE
    do {
        time_t t;
        struct tm tm1;
        struct tm *tmp;
        char date_string[30];
        t = time(NULL);
        tmp = localtime_r(&t, &tm1);
        if (tmp != NULL) {
            strftime(date_string, sizeof(date_string), "%F %T ", tmp);
            fputs(date_string, output_stream);
        }
    } while (0);
    #endif

    #if PINELOG_SHOW_LEVEL
    do {
        static const char *level_strings[] = {
            PINELOG_FATAL_STR,
            PINELOG_ERROR_STR,
            PINELOG_WARNING_STR,
            PINELOG_INFO_STR,
            PINELOG_DEBUG_STR,
            PINELOG_TRACE_STR,
        };

        fputs(level_strings[level], output_stream);
        fputs(": ", output_stream);
    } while (0);
    #endif

    #if PINELOG_SHOW_BACKTRACE
        fprintf(output_stream, "%s:%d ", file, line);
    #endif

    va_start(ap, fmt);
    vfprintf(output_stream, fmt, ap);
    va_end(ap);
    // Append a trailing newline to flush the log message
    fputs("\n", output_stream);
}
