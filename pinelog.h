/*
 * Pinelog lightweight logging library
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file logging.h
 * @brief Logging utility library
 *
 * This file contains the prototypes for the pinelog logging library
 * used by any programs that need to log messages.
 *
 * @author Nirenjan Krishnan (nirenjan@nirenjan.org)
 */
#ifndef LOGGING_H
#define LOGGING_H

#include "config.h"
#include <stdio.h>
#ifndef PINELOG_TEST
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logging levels
 *
 * The log levels indicate the lowest severity level that will actually be
 * logged to the logging framework.
 */
enum {
    /** No messages will be logged */
    PINELOG_LVL_NONE = -1,

    /** Only fatal messages will be logged */
    PINELOG_LVL_FATAL,

    /** Error messages. This is the default log level */
    PINELOG_LVL_ERROR,

    /** Warning messages */
    PINELOG_LVL_WARNING,

    /** Informational messages */
    PINELOG_LVL_INFO,

    /** Debug messages */
    PINELOG_LVL_DEBUG,

    /** Trace messages */
    PINELOG_LVL_TRACE,
};

/**
 * @brief Set the default log level and output stream
 */
void pinelog_set_defaults(void);

/**
 * @brief Close the output stream and terminate the logs
 */
void pinelog_close_output_stream(void);

#ifdef PINELOG_TEST
/**
 * @brief Get the pointer to the output stream. Only used in test harness.
 *
 * @returns FILE pointer to output stream
 */
FILE * pinelog_get_output_stream(void);
#endif

/**
 * @brief Set the output stream. Must be a FILE pointer.
 *
 * @param[in]   stream  Pointer to the output stream
 *
 * @returns 0 on success, EINVAL if the pointer is not valid.
 */
int pinelog_set_output_stream(FILE *stream);

/**
 * @brief Set the output file.
 *
 * @param[in]   file    Filename to write to
 *
 * @returns 0 on success, EINVAL if the filename pointer is not valid, other
 * error if the file could not be opened for writing.
 */
int pinelog_set_output_file(const char *file);

/**
 * @brief Set the logging level
 *
 * @param[in]   level   Level to filter
 *
 * @returns 0 on success, EINVAL if the level is not valid
 */
int pinelog_set_level(int level);

/**
 * @brief Get the logging level
 *
 * @returns the configured logging level
 */
int pinelog_get_level(void);

/**
 * @brief Log a message to the logger
 *
 * This is the actual function that logs the message. The application should
 * never need to call this directly, but instead, should always use the
 * \code PINELOG_* macros.
 *
 * @param[in]   level   Level to log the message at
 * @param[in]   fmt     Format string
 *
 * @returns None
 */
#if HAVE_FUNC_ATTRIBUTE_FORMAT
__attribute__((format(printf, 4, 5)))
#endif

void pinelog_log_message(int level, const char *file, int line, const char *fmt, ...);

// Test harness will redefine pinelog_exit
#ifndef PINELOG_TEST
#define pinelog_exit exit
#endif

#define PINELOG_FATAL(fmt, ...) do { \
    if (PINELOG_LVL_FATAL <= pinelog_get_level()) { \
        pinelog_log_message(PINELOG_LVL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
    pinelog_exit(1); \
} while (0)

#define PINELOG_ERROR(fmt, ...) do { \
    if (PINELOG_LVL_ERROR <= pinelog_get_level()) { \
        pinelog_log_message(PINELOG_LVL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
} while (0)

#define PINELOG_WARN(fmt, ...) do { \
    if (PINELOG_LVL_WARNING <= pinelog_get_level()) { \
        pinelog_log_message(PINELOG_LVL_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
} while(0)

#define PINELOG_INFO(fmt, ...) do { \
    if (PINELOG_LVL_INFO <= pinelog_get_level()) { \
        pinelog_log_message(PINELOG_LVL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
} while(0)

#define PINELOG_DEBUG(fmt, ...) do { \
    if (PINELOG_LVL_DEBUG <= pinelog_get_level()) { \
        pinelog_log_message(PINELOG_LVL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
} while(0)

#define PINELOG_TRACE(fmt, ...) do { \
    if (PINELOG_LVL_TRACE <= pinelog_get_level()) { \
        pinelog_log_message(PINELOG_LVL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
} while(0)

#ifdef __cplusplus
}
#endif

#endif // !defined LOGGING_H
