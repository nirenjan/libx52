/*
 * Saitek X52 Pro MFD & LED driver - CLI test harness
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libx52.h"

extern int run_main(int argc, char **argv);

/* Wrapper functions for libx52 */
int libx52_init(libx52_device **dev)
{
    function_called();
    *dev = NULL;
    return mock();
}

int libx52_connect(libx52_device *dev)
{
    function_called();
    return mock();
}

int libx52_update(libx52_device *dev)
{
    return LIBX52_SUCCESS;
}

void libx52_exit(libx52_device *dev)
{
    return;
}

const char *libx52_strerror(libx52_error_code rc)
{
    function_called();
    return "";
}

int libx52_set_text(libx52_device *x52, uint8_t line, const char *text, uint8_t length)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(line);
    check_expected(text);
    check_expected(length);

    return mock();
}

int libx52_set_led_state(libx52_device *x52, libx52_led_id id, libx52_led_state state)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(id);
    check_expected(state);

    return mock();
}

int libx52_set_clock(libx52_device *x52, time_t time, int local)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(time);
    check_expected(local);

    return mock();
}

int libx52_set_clock_timezone(libx52_device *x52, libx52_clock_id clock, int offset)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(clock);
    check_expected(offset);

    return mock();
}

int libx52_set_clock_format(libx52_device *x52, libx52_clock_id clock, libx52_clock_format format)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(clock);
    check_expected(format);

    return mock();
}

int libx52_set_time(libx52_device *x52, uint8_t hour, uint8_t minute)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(hour);
    check_expected(minute);

    return mock();
}

int libx52_set_date(libx52_device *x52, uint8_t dd, uint8_t mm, uint8_t yy)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(dd);
    check_expected(mm);
    check_expected(yy);

    return mock();
}

int libx52_set_date_format(libx52_device *x52, libx52_date_format format)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(format);

    return mock();
}

int libx52_set_brightness(libx52_device *x52, uint8_t mfd, uint16_t brightness)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(mfd);
    check_expected(brightness);

    return mock();
}

int libx52_set_shift(libx52_device *x52, uint8_t state)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(state);

    return mock();
}

int libx52_set_blink(libx52_device *x52, uint8_t state)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(state);

    return mock();
}

int libx52_vendor_command(libx52_device *x52, uint16_t index, uint16_t value)
{
    function_called();
    assert_ptr_equal(x52, NULL);
    check_expected(index);
    check_expected(value);

    return mock();
}

#include "test_x52_cli_tests.c"
#define TEST_LIST

const struct CMUnitTest tests[] = {
    #include "test_x52_cli_tests.c"
};

int main(int argc, char **argv)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}

