/*
 * Pinelog lightweight logging library
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
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

/*
 * Buffer size is used to create an automatic buffer. Set to 0 to disable,
 * and write directly to the output stream. The drawback is that log messages
 * from multiple threads may be interleaved without a buffer.
 */
#ifndef PINELOG_BUFFER_SZ
#define PINELOG_BUFFER_SZ 0
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

/** Number of modules */
static int num_modules = 0;

/** Per module logging levels */
static int *module_level = NULL;

/** Module names */
static const char **module_name = NULL;

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

int pinelog_init(int count) {
    int rc = 0;
    int i;

    if (count <= 0) {
        return EINVAL;
    }

    num_modules = count;
    free(module_level);
    free(module_name);

    module_level = calloc(sizeof(*module_level), count);
    if (module_level == NULL) {
        rc = errno;
        goto cleanup;
    }

    module_name = calloc(sizeof(*module_name), count);
    if (module_name == NULL) {
        rc = errno;
        goto cleanup;
    }

    for (i = 0; i < count; i++) {
        module_level[i] = PINELOG_LVL_NOTSET;
    }

    return 0;

cleanup:
    num_modules = 0;
    free(module_level);
    free(module_name);
    return rc;
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

int pinelog_get_module_level(int module)
{
    int global_level = pinelog_get_level();
    int level = global_level;

    if (module >= 0 && module < num_modules) {
        level = module_level[module];
        if (level == PINELOG_LVL_NOTSET) {
            level = global_level;
        }
    }

    return level;
}

int pinelog_set_module_level(int module, int level)
{
    if (module < 0 || module > num_modules) {
        if (module == PINELOG_MODULE_GLOBAL) {
            return pinelog_set_level(level);
        }

        return ERANGE;
    }

    if (level < PINELOG_LVL_NOTSET || level > PINELOG_LVL_TRACE) {
        return EINVAL;
    }

    module_level[module] = level;
    return 0;
}

int pinelog_setup_module(int module, const char *name)
{
    if (module < 0 || module > num_modules) {
        return ERANGE;
    }

    if (name == NULL) {
        return EINVAL;
    }

    module_name[module] = name;
    return 0;
}

/**********************************************************************
 * Log the message to the output stream
 *********************************************************************/
void pinelog_log_message(int module, int level, const char *file, int line, const char *fmt, ...)
{
    #if PINELOG_BUFFER_SZ
    char output_buffer[PINELOG_BUFFER_SZ];
    size_t buf_pos = 0;
    #endif

    va_list ap;

    /* Break out if the module is not in the acceptable range */
    if ((module < 0 || module >= num_modules ) && module != PINELOG_MODULE_GLOBAL) {
        return;
    }

    /* Reset the level if we are in a module and the level is NOTSET */
    /* Don't log anything if the level is not severe enough */
    if (level > pinelog_get_module_level(module) || level < 0) {
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
        t = time(NULL);
        tmp = localtime_r(&t, &tm1);
        if (tmp != NULL) {
            #if PINELOG_BUFFER_SZ
            buf_pos += strftime(output_buffer + buf_pos, sizeof(output_buffer) - buf_pos,
                                "%F %T ", tmp);
            #else
            char date_string[30];
            strftime(date_string, sizeof(date_string), "%F %T ", tmp);
            fputs(date_string, output_stream);
            #endif
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

        #if PINELOG_BUFFER_SZ
        buf_pos += snprintf(output_buffer + buf_pos, sizeof(output_buffer) - buf_pos,
                            "%s: ", level_strings[level]);
        #else
        fputs(level_strings[level], output_stream);
        fputs(": ", output_stream);
        #endif
    } while (0);
    #endif

    #if PINELOG_SHOW_BACKTRACE
        #if PINELOG_BUFFER_SZ
        buf_pos += snprintf(output_buffer + buf_pos, sizeof(output_buffer) - buf_pos,
                            "%s:%d ", file, line);
        #else
        fprintf(output_stream, "%s:%d ", file, line);
        #endif
    #endif

    /* Set the module name if it is not the root */
    if (module != PINELOG_MODULE_GLOBAL && module_name[module] != NULL) {
        #if PINELOG_BUFFER_SZ
        buf_pos += snprintf(output_buffer + buf_pos, sizeof(output_buffer) - buf_pos,
                            "%s: ", module_name[module]);
        #else
        fprintf(output_stream, "%s: ", module_name[module]);
        #endif
    }

    va_start(ap, fmt);
    #if PINELOG_BUFFER_SZ
    buf_pos += vsnprintf(output_buffer + buf_pos, sizeof(output_buffer) - buf_pos,
                        fmt, ap);
    // Append a trailing newline to flush the log message
    buf_pos += snprintf(output_buffer + buf_pos, sizeof(output_buffer) - buf_pos, "\n");
    fputs(output_buffer, output_stream);
    #else
    vfprintf(output_stream, fmt, ap);
    // Append a trailing newline to flush the log message
    fputs("\n", output_stream);
    #endif
    va_end(ap);
}
