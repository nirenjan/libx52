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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**********************************************************************
 * Global variables
 *********************************************************************/
// Test ID (of current test case)
static unsigned int test_id;

// Observed output stream
static FILE *observed_stream_w;
static FILE *observed_stream_r;

// Temporary pipe for observed data
static char observed_fifo[NAME_MAX];

// Buffer for expected output
static char expected_output[1024];
static size_t expected_len;

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

static int test_setup(int level, int filter, const char *file, int line)
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
            expected_len += snprintf(&expected_output[expected_len],
                                     sizeof(expected_output) - expected_len,
                                     "%s:%d ", file, line);
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

#define TEST_LOG(lvl, filter, fmt, ...) do { \
    if (test_setup(PINELOG_LVL_ ## lvl, PINELOG_LVL_ ## filter, \
               __FILE__, __LINE__)) \
       expected_len += snprintf(&expected_output[expected_len], \
                                sizeof(expected_output) - expected_len, \
                                fmt "\n", ##__VA_ARGS__); \
    PINELOG_ ## lvl (fmt, ##__VA_ARGS__); \
    test_teardown("Log " #lvl " filter " #filter); \
} while(0)

#define TEST(filter, fmt, ...) do { \
    pinelog_set_level(PINELOG_LVL_ ## filter); \
    TEST_LOG(TRACE, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(DEBUG, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(INFO, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(WARNING, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(ERROR, filter, fmt, ##__VA_ARGS__); \
    TEST_LOG(FATAL, filter, fmt, ##__VA_ARGS__); \
} while (0)

int main(int argc, char **argv)
{
    int fifo_fd_r, fifo_fd_w;
    snprintf(observed_fifo, sizeof(observed_fifo), "%s.fifo", argv[0]);
    mkfifo(observed_fifo, 0777);

    fifo_fd_r = open(observed_fifo, O_RDONLY | O_NONBLOCK);
    fifo_fd_w = open(observed_fifo, O_WRONLY | O_NONBLOCK);
    observed_stream_r = fdopen(fifo_fd_r, "r");
    observed_stream_w = fdopen(fifo_fd_w, "w");

    verify_defaults();

    pinelog_set_output_stream(observed_stream_w);
    TEST(TRACE, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    TEST(DEBUG, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    TEST(INFO, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    TEST(WARNING, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    TEST(ERROR, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    TEST(FATAL, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);
    TEST(NONE, "testing %s... %d, %f, %u", "testing", -1, 0.0, 1);

    printf("1..%u\n", test_id);

    fclose(observed_stream_w);
    fclose(observed_stream_r);
    close(fifo_fd_w);
    close(fifo_fd_r);
    unlink(observed_fifo);

    return 0;
}
