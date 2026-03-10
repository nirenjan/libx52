/*
 * Pinelog lightweight logging library - test harness
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: MIT
 */

#include "pinelog.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/**********************************************************************
 * Global variables
 *********************************************************************/
// Test ID (of current test case)
static unsigned int test_id;

// Observed output stream
static FILE *observed_stream_w;
static FILE *observed_stream_r;

// Buffer for expected output
static char expected_output[1024];
static size_t expected_len;

time_t time(time_t *p)
{
    // Override the time function from libc
    return 1636671600;
}

static void test_case(const char *desc, bool test)
{
    test_id++;
    if (test) {
        printf("ok %u %s\n", test_id, desc);
    } else {
        printf("not ok %u %s\n", test_id, desc);
    }
}

static void pinelog_exit(int status)
{
    fprintf(observed_stream_w, "EXIT(%d)\n", status);
    expected_len += snprintf(&expected_output[expected_len],
                             sizeof(expected_output) - expected_len,
                             "EXIT(%d)\n", status);
}

static void dump_data(const char *type, size_t len, char *data)
{
    char *line;
    printf("# %s (%lu bytes):\n", type, len);
    line = strtok(data, "\n");
    while (line != NULL) {
        printf("#\t%s\n", line);
        line = strtok(NULL, "\n");
    }
    printf("\n");
}

static int test_setup(int module, int level, int filter, const char *file, int line)
{
    expected_len = 0;
    memset(expected_output, 0, sizeof(expected_output));

    if (level <= filter) {
        if (PINELOG_SHOW_DATE) {
            time_t t;
            struct tm *tmp;

            t = time(NULL);
            tmp = localtime(&t);
            expected_len += strftime(&expected_output[expected_len],
                                     sizeof(expected_output) - expected_len,
                                     "%F %T ", tmp);
        }

        if (PINELOG_SHOW_LEVEL) {
            const char * level_string[] = {
                PINELOG_FATAL_STR,
                PINELOG_ERROR_STR,
                PINELOG_WARNING_STR,
                PINELOG_INFO_STR,
                PINELOG_DEBUG_STR,
                PINELOG_TRACE_STR,
            };
            expected_len += snprintf(&expected_output[expected_len],
                                     sizeof(expected_output) - expected_len,
                                     "%s: ", level_string[level]);
        }

        if (PINELOG_SHOW_BACKTRACE) {
            char * basename = NULL;
            #if defined __has_builtin
            #if __has_builtin(__builtin_strrchr)
            basename = strrchr(file, '/');
            #endif
            #endif

            if (basename != NULL) {
                basename++;
            } else {
                // Override the const
                basename = (char *)file;
            }
            expected_len += snprintf(&expected_output[expected_len],
                                     sizeof(expected_output) - expected_len,
                                     "%s:%d ", basename, line);
        }

        if (module >= 0) {
            static const char * modules[] = {"foo", "bar"};
            expected_len += snprintf(&expected_output[expected_len],
                                     sizeof(expected_output) - expected_len,
                                     "%s: ", modules[module]);
        }
        return 1;
    }

    return 0;
}

static void test_teardown(const char *desc)
{
    // Compare the output
    static char observed[1024];
    size_t observed_len;
    int result;

    observed_len = fread(observed, 1, sizeof(observed), observed_stream_r);

    result = ((expected_len == observed_len) &&
              (memcmp(expected_output, observed, expected_len) == 0));
    test_case(desc, result);
    if (!result) {
        dump_data("expected", expected_len, expected_output);
        dump_data("observed", observed_len, observed);
    }
}

static void verify_defaults(void)
{
    test_case("Get default output stream",
              pinelog_get_output_stream() == PINELOG_DEFAULT_STREAM);
    test_case("Get default logging level",
              pinelog_get_level() == PINELOG_DEFAULT_LEVEL);
}

#define PINELOG_WARNING PINELOG_WARN

#define TEST_LOG(module, lvl, filter, fmt, ...) do { \
    if (test_setup(module, PINELOG_LVL_ ## lvl, PINELOG_LVL_ ## filter, \
               __FILE__, __LINE__)) \
       expected_len += snprintf(&expected_output[expected_len], \
                                sizeof(expected_output) - expected_len, \
                                fmt "\n", ##__VA_ARGS__); \
    PINELOG_ ## lvl (fmt, ##__VA_ARGS__); \
    switch (module) { \
        case -1: test_teardown("Global Log " #lvl " filter " #filter); break; \
        case 0: test_teardown("Module foo Log " #lvl " filter " #filter); break; \
        case 1: test_teardown("Module bar Log " #lvl " filter " #filter); break; \
        default: test_teardown("Module ??? Log " #lvl " filter " #filter); break; \
    } \
} while(0)

#define TEST(module, filter, fmt, ...) do { \
    pinelog_set_level(PINELOG_LVL_ ## filter); \
    TEST_LOG(module, TRACE, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(module, DEBUG, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(module, INFO, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(module, WARNING, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(module, ERROR, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(module, FATAL, filter, fmt, ##__VA_ARGS__); \
} while (0)

#if defined __has_attribute
#   if __has_attribute(noreturn)
        __attribute__((noreturn))
#   endif
#endif
static void tap_bailout(const char *msg)
{
    printf("Bail out! Error %d %s:%s\n", errno, msg, strerror(errno));
    exit(1);
}

int main(int argc, char **argv)
{
    int fifo_fd[2];
    int flags;

    if (pipe(fifo_fd) != 0) {
        tap_bailout("creating pipe");
    }

    /* Set the pipe to be non-blocking */
    flags = fcntl(fifo_fd[0], F_GETFL);
    if (flags < 0) {
        tap_bailout("fetching read pipe flags");
    }
    if (fcntl(fifo_fd[0], F_SETFL, flags | O_NONBLOCK) < 0) {
        tap_bailout("setting read pipe to nonblocking");
    }

    flags = fcntl(fifo_fd[1], F_GETFL);
    if (flags < 0) {
        tap_bailout("fetching write pipe flags");
    }
    if (fcntl(fifo_fd[1], F_SETFL, flags | O_NONBLOCK) < 0) {
        tap_bailout("setting write pipe to nonblocking");
    }

    observed_stream_r = fdopen(fifo_fd[0], "r");
    observed_stream_w = fdopen(fifo_fd[1], "w");

    verify_defaults();

    pinelog_init(2);
    pinelog_setup_module(0, "foo");
    pinelog_setup_module(1, "bar");

    pinelog_set_output_stream(observed_stream_w);
    for (int i = -1; i < 2; i++) {
        #ifdef PINELOG_MODULE
        #undef PINELOG_MODULE
        #define PINELOG_MODULE i
        #endif
        TEST(i, TRACE, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
        TEST(i, DEBUG, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
        TEST(i, INFO, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
        TEST(i, WARNING, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
        TEST(i, ERROR, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
        TEST(i, FATAL, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
        TEST(i, NONE, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    }

    printf("1..%u\n", test_id);

    pinelog_close_output_stream();
    fclose(observed_stream_r);
    close(fifo_fd[0]);
    close(fifo_fd[1]);

    return 0;
}
