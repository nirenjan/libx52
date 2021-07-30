/*
 * Saitek X52 Pro MFD & LED driver - CLI test suite
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef TEST_LIST
// Setup the test case function
#define TEST_CASE(tc)   static void tc(void **state)
#define TEST_DEF 1
// Function header, this calls the corresponding libx52 function, and expects
// a certain number of calls to that function
#define SETUP(...) \
    int rc; \
    char *argv[] = {"x52cli", __VA_ARGS__};

#define FUNC(fn, count) \
    expect_function_calls(libx52_ ## fn, count); \
    will_return_count(libx52_ ## fn, LIBX52_SUCCESS, count);

#define EXPECT(fn, param, value) expect_value(libx52_ ## fn, param, value);
#define EXPECT_STRING(fn, param, value) expect_string(libx52_ ##fn, param, value);
#define FINISH() \
    rc = run_main(sizeof(argv)/sizeof(argv[0]), argv); \
    assert_int_equal(rc, LIBX52_SUCCESS);

#else // !defined TEST_LIST
#define TEST_CASE(tc) cmocka_unit_test(tc),
#define TEST_DEF 0
#endif // defined TEST_LIST

/* LED test cases {{{ */
TEST_CASE(led_fire_off)
#if TEST_DEF
{
    SETUP("led", "fire", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_fire_on)
#if TEST_DEF
{
    SETUP("led", "fire", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_fire_red)
#if TEST_DEF
{
    SETUP("led", "fire", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_fire_amber)
#if TEST_DEF
{
    SETUP("led", "fire", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_fire_green)
#if TEST_DEF
{
    SETUP("led", "fire", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_a_off)
#if TEST_DEF
{
    SETUP("led", "a", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_a_on)
#if TEST_DEF
{
    SETUP("led", "a", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_a_red)
#if TEST_DEF
{
    SETUP("led", "a", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_a_amber)
#if TEST_DEF
{
    SETUP("led", "a", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_a_green)
#if TEST_DEF
{
    SETUP("led", "a", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_b_off)
#if TEST_DEF
{
    SETUP("led", "b", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_b_on)
#if TEST_DEF
{
    SETUP("led", "b", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_b_red)
#if TEST_DEF
{
    SETUP("led", "b", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_b_amber)
#if TEST_DEF
{
    SETUP("led", "b", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_b_green)
#if TEST_DEF
{
    SETUP("led", "b", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_d_off)
#if TEST_DEF
{
    SETUP("led", "d", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_d_on)
#if TEST_DEF
{
    SETUP("led", "d", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_d_red)
#if TEST_DEF
{
    SETUP("led", "d", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_d_amber)
#if TEST_DEF
{
    SETUP("led", "d", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_d_green)
#if TEST_DEF
{
    SETUP("led", "d", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_e_off)
#if TEST_DEF
{
    SETUP("led", "e", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_e_on)
#if TEST_DEF
{
    SETUP("led", "e", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_e_red)
#if TEST_DEF
{
    SETUP("led", "e", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_e_amber)
#if TEST_DEF
{
    SETUP("led", "e", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_e_green)
#if TEST_DEF
{
    SETUP("led", "e", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_t1_off)
#if TEST_DEF
{
    SETUP("led", "t1", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_t1_on)
#if TEST_DEF
{
    SETUP("led", "t1", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_t1_red)
#if TEST_DEF
{
    SETUP("led", "t1", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_t1_amber)
#if TEST_DEF
{
    SETUP("led", "t1", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_t1_green)
#if TEST_DEF
{
    SETUP("led", "t1", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_t2_off)
#if TEST_DEF
{
    SETUP("led", "t2", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_t2_on)
#if TEST_DEF
{
    SETUP("led", "t2", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_t2_red)
#if TEST_DEF
{
    SETUP("led", "t2", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_t2_amber)
#if TEST_DEF
{
    SETUP("led", "t2", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_t2_green)
#if TEST_DEF
{
    SETUP("led", "t2", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_t3_off)
#if TEST_DEF
{
    SETUP("led", "t3", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_t3_on)
#if TEST_DEF
{
    SETUP("led", "t3", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_t3_red)
#if TEST_DEF
{
    SETUP("led", "t3", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_t3_amber)
#if TEST_DEF
{
    SETUP("led", "t3", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_t3_green)
#if TEST_DEF
{
    SETUP("led", "t3", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_pov_off)
#if TEST_DEF
{
    SETUP("led", "pov", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_pov_on)
#if TEST_DEF
{
    SETUP("led", "pov", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_pov_red)
#if TEST_DEF
{
    SETUP("led", "pov", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_pov_amber)
#if TEST_DEF
{
    SETUP("led", "pov", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_pov_green)
#if TEST_DEF
{
    SETUP("led", "pov", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

TEST_CASE(led_throttle_off)
#if TEST_DEF
{
    SETUP("led", "throttle", "off")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
}
#endif

TEST_CASE(led_throttle_on)
#if TEST_DEF
{
    SETUP("led", "throttle", "on")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
}
#endif

TEST_CASE(led_throttle_red)
#if TEST_DEF
{
    SETUP("led", "throttle", "red")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
}
#endif

TEST_CASE(led_throttle_amber)
#if TEST_DEF
{
    SETUP("led", "throttle", "amber")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
}
#endif

TEST_CASE(led_throttle_green)
#if TEST_DEF
{
    SETUP("led", "throttle", "green")
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
}
#endif

/* }}} */

/* Brightness test cases {{{ */
TEST_CASE(brightness_mfd_0)
#if TEST_DEF
{
    SETUP("bri", "mfd", "0")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 0)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_1)
#if TEST_DEF
{
    SETUP("bri", "mfd", "1")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 1)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_2)
#if TEST_DEF
{
    SETUP("bri", "mfd", "2")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 2)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_126)
#if TEST_DEF
{
    SETUP("bri", "mfd", "126")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 126)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_127)
#if TEST_DEF
{
    SETUP("bri", "mfd", "127")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 127)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_128)
#if TEST_DEF
{
    SETUP("bri", "mfd", "128")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 128)
    FINISH()
}
#endif

TEST_CASE(brightness_led_0)
#if TEST_DEF
{
    SETUP("bri", "led", "0")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 0)
    FINISH()
}
#endif

TEST_CASE(brightness_led_1)
#if TEST_DEF
{
    SETUP("bri", "led", "1")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 1)
    FINISH()
}
#endif

TEST_CASE(brightness_led_2)
#if TEST_DEF
{
    SETUP("bri", "led", "2")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 2)
    FINISH()
}
#endif

TEST_CASE(brightness_led_126)
#if TEST_DEF
{
    SETUP("bri", "led", "126")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 126)
    FINISH()
}
#endif

TEST_CASE(brightness_led_127)
#if TEST_DEF
{
    SETUP("bri", "led", "127")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 127)
    FINISH()
}
#endif

TEST_CASE(brightness_led_128)
#if TEST_DEF
{
    SETUP("bri", "led", "128")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 128)
    FINISH()
}
#endif

/* }}} */

/* Indicator test cases {{{ */
TEST_CASE(indicator_blink_on)
#if TEST_DEF
{
    SETUP("blink", "on")
    FUNC(set_blink, 1)
    EXPECT(set_blink, state, 1)
    FINISH()
}
#endif

TEST_CASE(indicator_blink_off)
#if TEST_DEF
{
    SETUP("blink", "off")
    FUNC(set_blink, 1)
    EXPECT(set_blink, state, 0)
    FINISH()
}
#endif

TEST_CASE(indicator_shift_on)
#if TEST_DEF
{
    SETUP("shift", "on")
    FUNC(set_shift, 1)
    EXPECT(set_shift, state, 1)
    FINISH()
}
#endif

TEST_CASE(indicator_shift_off)
#if TEST_DEF
{
    SETUP("shift", "off")
    FUNC(set_shift, 1)
    EXPECT(set_shift, state, 0)
    FINISH()
}
#endif

/* }}} */

/* MFD text tests {{{ */
TEST_CASE(mfd_0_1)
#if TEST_DEF
{
    SETUP("mfd", "0", "a")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "a")
    EXPECT(set_text, length, 1)
    FINISH()
}
#endif

TEST_CASE(mfd_0_2)
#if TEST_DEF
{
    SETUP("mfd", "0", "ab")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "ab")
    EXPECT(set_text, length, 2)
    FINISH()
}
#endif

TEST_CASE(mfd_1_1)
#if TEST_DEF
{
    SETUP("mfd", "1", "a")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "a")
    EXPECT(set_text, length, 1)
    FINISH()
}
#endif

TEST_CASE(mfd_1_2)
#if TEST_DEF
{
    SETUP("mfd", "1", "ab")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "ab")
    EXPECT(set_text, length, 2)
    FINISH()
}
#endif

TEST_CASE(mfd_2_1)
#if TEST_DEF
{
    SETUP("mfd", "2", "a")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "a")
    EXPECT(set_text, length, 1)
    FINISH()
}
#endif

TEST_CASE(mfd_2_2)
#if TEST_DEF
{
    SETUP("mfd", "2", "ab")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "ab")
    EXPECT(set_text, length, 2)
    FINISH()
}
#endif

/* }}} */

/* Clock tests {{{ */
TEST_CASE(clock_local_12hr_ddmmyy)
#if TEST_DEF
{
    SETUP("clock", "local", "12hr", "ddmmyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
}
#endif

TEST_CASE(clock_local_12hr_mmddyy)
#if TEST_DEF
{
    SETUP("clock", "local", "12hr", "mmddyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
}
#endif

TEST_CASE(clock_local_12hr_yymmdd)
#if TEST_DEF
{
    SETUP("clock", "local", "12hr", "yymmdd")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
}
#endif

TEST_CASE(clock_local_24hr_ddmmyy)
#if TEST_DEF
{
    SETUP("clock", "local", "24hr", "ddmmyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
}
#endif

TEST_CASE(clock_local_24hr_mmddyy)
#if TEST_DEF
{
    SETUP("clock", "local", "24hr", "mmddyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
}
#endif

TEST_CASE(clock_local_24hr_yymmdd)
#if TEST_DEF
{
    SETUP("clock", "local", "24hr", "yymmdd")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
}
#endif

TEST_CASE(clock_gmt_12hr_ddmmyy)
#if TEST_DEF
{
    SETUP("clock", "gmt", "12hr", "ddmmyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
}
#endif

TEST_CASE(clock_gmt_12hr_mmddyy)
#if TEST_DEF
{
    SETUP("clock", "gmt", "12hr", "mmddyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
}
#endif

TEST_CASE(clock_gmt_12hr_yymmdd)
#if TEST_DEF
{
    SETUP("clock", "gmt", "12hr", "yymmdd")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
}
#endif

TEST_CASE(clock_gmt_24hr_ddmmyy)
#if TEST_DEF
{
    SETUP("clock", "gmt", "24hr", "ddmmyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
}
#endif

TEST_CASE(clock_gmt_24hr_mmddyy)
#if TEST_DEF
{
    SETUP("clock", "gmt", "24hr", "mmddyy")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
}
#endif

TEST_CASE(clock_gmt_24hr_yymmdd)
#if TEST_DEF
{
    SETUP("clock", "gmt", "24hr", "yymmdd")
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
}
#endif
/* }}} */

/* Clock offset tests {{{ */
TEST_CASE(offset_2__30_12hr)
#if TEST_DEF
{
    SETUP("offset", "2", "-30", "12hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(offset_2__30_24hr)
#if TEST_DEF
{
    SETUP("offset", "2", "-30", "24hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(offset_2_0_12hr)
#if TEST_DEF
{
    SETUP("offset", "2", "0", "12hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(offset_2_0_24hr)
#if TEST_DEF
{
    SETUP("offset", "2", "0", "24hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(offset_2_30_12hr)
#if TEST_DEF
{
    SETUP("offset", "2", "30", "12hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(offset_2_30_24hr)
#if TEST_DEF
{
    SETUP("offset", "2", "30", "24hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(offset_3__30_12hr)
#if TEST_DEF
{
    SETUP("offset", "3", "-30", "12hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(offset_3__30_24hr)
#if TEST_DEF
{
    SETUP("offset", "3", "-30", "24hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(offset_3_0_12hr)
#if TEST_DEF
{
    SETUP("offset", "3", "0", "12hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(offset_3_0_24hr)
#if TEST_DEF
{
    SETUP("offset", "3", "0", "24hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(offset_3_30_12hr)
#if TEST_DEF
{
    SETUP("offset", "3", "30", "12hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(offset_3_30_24hr)
#if TEST_DEF
{
    SETUP("offset", "3", "30", "24hr")
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

/* }}} */

/* Raw time tests {{{ */
TEST_CASE(raw_time_0_0_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_0_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_1_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "1", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 1)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_1_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "1", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 1)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_1_0_12hr)
#if TEST_DEF
{
    SETUP("time", "1", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 1)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_1_0_24hr)
#if TEST_DEF
{
    SETUP("time", "1", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 1)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_2_0_12hr)
#if TEST_DEF
{
    SETUP("time", "2", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 2)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_2_0_24hr)
#if TEST_DEF
{
    SETUP("time", "2", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 2)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

/* }}} */

/* Raw date tests {{{ */
TEST_CASE(raw_date_ddmmyy)
#if TEST_DEF
{
    SETUP("date", "1", "2", "3", "ddmmyy")
    FUNC(set_date, 1)
    EXPECT(set_date, dd, 1)
    EXPECT(set_date, mm, 2)
    EXPECT(set_date, yy, 3)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
}
#endif

TEST_CASE(raw_date_mmddyy)
#if TEST_DEF
{
    SETUP("date", "1", "2", "3", "mmddyy")
    FUNC(set_date, 1)
    EXPECT(set_date, dd, 1)
    EXPECT(set_date, mm, 2)
    EXPECT(set_date, yy, 3)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
}
#endif

TEST_CASE(raw_date_yymmdd)
#if TEST_DEF
{
    SETUP("date", "1", "2", "3", "yymmdd")
    FUNC(set_date, 1)
    EXPECT(set_date, dd, 1)
    EXPECT(set_date, mm, 2)
    EXPECT(set_date, yy, 3)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
}
#endif
/* }}} */

/* Vendor command tests {{{ */
TEST_CASE(vendor_command_hex)
#if TEST_DEF
{
    SETUP("raw", "0x0123", "0x4567")
    FUNC(vendor_command, 1)
    EXPECT(vendor_command, index, 0x0123)
    EXPECT(vendor_command, value, 0x4567)
    FINISH()
}
#endif

TEST_CASE(vendor_command_oct)
#if TEST_DEF
{
    SETUP("raw", "0123", "0456")
    FUNC(vendor_command, 1)
    EXPECT(vendor_command, index, 0123)
    EXPECT(vendor_command, value, 0456)
    FINISH()
}
#endif

TEST_CASE(vendor_command_dec)
#if TEST_DEF
{
    SETUP("raw", "123", "456")
    FUNC(vendor_command, 1)
    EXPECT(vendor_command, index, 123)
    EXPECT(vendor_command, value, 456)
    FINISH()
}
#endif

/* }}} */

#undef TEST_CASE
#undef TEST_DEF
