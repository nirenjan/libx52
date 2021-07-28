/*
 * Pinelog lightweight logging library - test harness
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: MIT
 */


#define pinelog_exit(_)
#include "pinelog.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

#define BENCH_COUNT 100000

static void print_time_difference(const char *type, struct timespec *ts)
{
    struct timespec ret;
    uint64_t timeper;
    uint64_t tp_usec;
    uint64_t tp_nsec;

    // ts is a pointer to a 2 element array, second is always later
    ret.tv_sec = ts[1].tv_sec - ts[0].tv_sec;
    ret.tv_nsec = ts[1].tv_nsec - ts[0].tv_nsec;

    if (ts[0].tv_nsec > ts[1].tv_nsec) {
        ret.tv_nsec += 1000000000;
        ret.tv_sec--;
    }

    timeper = (ret.tv_sec * 1000000000 + ret.tv_nsec) / BENCH_COUNT;
    tp_usec = timeper / 1000;
    tp_nsec = timeper % 1000;

    printf("# %s %"PRIu64".%03"PRIu64"\u03BCs/log (Total %lu.%09lds)\n",
           type, tp_usec, tp_nsec, ret.tv_sec, ret.tv_nsec);
}

int main(int argc, char **argv)
{
    struct timespec ts_wall[2];
    struct timespec ts_cpu[2];
    int i;

    /* Set up defaults */
    pinelog_set_level(PINELOG_LVL_ERROR);
    pinelog_set_output_file("/dev/null");

    printf("# Timing logging for %u iterations\n", BENCH_COUNT);

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_cpu[0]) < 0) {
        perror("clock_gettime cputime 0");
        return 1;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &ts_wall[0]) < 0) {
        perror("clock_gettime monotonic 0");
        return 1;
    }

    for (i = 0; i < BENCH_COUNT; i++) {
        PINELOG_ERROR("Testing error log #%u of %u", i, BENCH_COUNT);
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_cpu[1]);
    clock_gettime(CLOCK_MONOTONIC, &ts_wall[1]);

    // Add a test plan and  to avoid the TAP harness from flagging this as a
    // failed test.
    puts("1..1");
    printf("ok 1 Benchmark pinelog %stimestamp, %slevel, %sbacktrace\n",
           PINELOG_SHOW_DATE ? "": "no ",
           PINELOG_SHOW_LEVEL ? "": "no ",
           PINELOG_SHOW_BACKTRACE ? "": "no ");
    print_time_difference("cpu time", ts_cpu);
    print_time_difference("wall time", ts_wall);

    return 0;
}
