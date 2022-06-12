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
#define TEST_DEF(x) x
// Function header, this calls the corresponding libx52 function, and expects
// a certain number of calls to that function
#define SETUP(...) \
    int rc; \
    char *argv[] = {"x52cli", __VA_ARGS__};

#define FUNC(fn, count) \
    expect_function_calls(libx52_ ## fn, count); \
    will_return_count(libx52_ ## fn, LIBX52_SUCCESS, count);

#define CONNECT_FUNCS() \
    FUNC(init, 1) \
    FUNC(connect, 1)

#define EXPECT(fn, param, value) expect_value(libx52_ ## fn, param, value);
#define EXPECT_STRING(fn, param, value) expect_string(libx52_ ##fn, param, value);
#define FINISH() \
    rc = run_main(sizeof(argv)/sizeof(argv[0]), argv); \
    assert_int_equal(rc, 0);

#define FAIL() \
    rc = run_main(sizeof(argv)/sizeof(argv[0]), argv); \
    assert_int_equal(rc, 1);

#else // !defined TEST_LIST
#define TEST_CASE(tc) cmocka_unit_test(tc),
#define TEST_DEF(x)
#endif // defined TEST_LIST

/* LED test cases {{{ */
TEST_CASE(led_fire_off)
TEST_DEF({
    SETUP("led", "fire", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_fire_on)
TEST_DEF({
    SETUP("led", "fire", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_fire_red)
TEST_DEF({
    SETUP("led", "fire", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_fire_amber)
TEST_DEF({
    SETUP("led", "fire", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_fire_green)
TEST_DEF({
    SETUP("led", "fire", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_a_off)
TEST_DEF({
    SETUP("led", "a", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_a_on)
TEST_DEF({
    SETUP("led", "a", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_a_red)
TEST_DEF({
    SETUP("led", "a", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_a_amber)
TEST_DEF({
    SETUP("led", "a", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_a_green)
TEST_DEF({
    SETUP("led", "a", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_A)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_b_off)
TEST_DEF({
    SETUP("led", "b", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_b_on)
TEST_DEF({
    SETUP("led", "b", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_b_red)
TEST_DEF({
    SETUP("led", "b", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_b_amber)
TEST_DEF({
    SETUP("led", "b", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_b_green)
TEST_DEF({
    SETUP("led", "b", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_B)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_d_off)
TEST_DEF({
    SETUP("led", "d", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_d_on)
TEST_DEF({
    SETUP("led", "d", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_d_red)
TEST_DEF({
    SETUP("led", "d", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_d_amber)
TEST_DEF({
    SETUP("led", "d", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_d_green)
TEST_DEF({
    SETUP("led", "d", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_D)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_e_off)
TEST_DEF({
    SETUP("led", "e", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_e_on)
TEST_DEF({
    SETUP("led", "e", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_e_red)
TEST_DEF({
    SETUP("led", "e", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_e_amber)
TEST_DEF({
    SETUP("led", "e", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_e_green)
TEST_DEF({
    SETUP("led", "e", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_E)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_t1_off)
TEST_DEF({
    SETUP("led", "t1", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_t1_on)
TEST_DEF({
    SETUP("led", "t1", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_t1_red)
TEST_DEF({
    SETUP("led", "t1", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_t1_amber)
TEST_DEF({
    SETUP("led", "t1", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_t1_green)
TEST_DEF({
    SETUP("led", "t1", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T1)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_t2_off)
TEST_DEF({
    SETUP("led", "t2", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_t2_on)
TEST_DEF({
    SETUP("led", "t2", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_t2_red)
TEST_DEF({
    SETUP("led", "t2", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_t2_amber)
TEST_DEF({
    SETUP("led", "t2", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_t2_green)
TEST_DEF({
    SETUP("led", "t2", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T2)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_t3_off)
TEST_DEF({
    SETUP("led", "t3", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_t3_on)
TEST_DEF({
    SETUP("led", "t3", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_t3_red)
TEST_DEF({
    SETUP("led", "t3", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_t3_amber)
TEST_DEF({
    SETUP("led", "t3", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_t3_green)
TEST_DEF({
    SETUP("led", "t3", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_T3)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_pov_off)
TEST_DEF({
    SETUP("led", "pov", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_pov_on)
TEST_DEF({
    SETUP("led", "pov", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_pov_red)
TEST_DEF({
    SETUP("led", "pov", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_pov_amber)
TEST_DEF({
    SETUP("led", "pov", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_pov_green)
TEST_DEF({
    SETUP("led", "pov", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_POV)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

TEST_CASE(led_throttle_off)
TEST_DEF({
    SETUP("led", "throttle", "off")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    FINISH()
})

TEST_CASE(led_throttle_on)
TEST_DEF({
    SETUP("led", "throttle", "on")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_ON)
    FINISH()
})

TEST_CASE(led_throttle_red)
TEST_DEF({
    SETUP("led", "throttle", "red")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_RED)
    FINISH()
})

TEST_CASE(led_throttle_amber)
TEST_DEF({
    SETUP("led", "throttle", "amber")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_AMBER)
    FINISH()
})

TEST_CASE(led_throttle_green)
TEST_DEF({
    SETUP("led", "throttle", "green")
    CONNECT_FUNCS()
    FUNC(set_led_state, 1)
    EXPECT(set_led_state, id, LIBX52_LED_THROTTLE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_GREEN)
    FINISH()
})

/* }}} */

/* Brightness test cases {{{ */
TEST_CASE(brightness_mfd_0)
TEST_DEF({
    SETUP("bri", "mfd", "0")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 0)
    FINISH()
})

TEST_CASE(brightness_mfd_1)
TEST_DEF({
    SETUP("bri", "mfd", "1")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 1)
    FINISH()
})

TEST_CASE(brightness_mfd_2)
TEST_DEF({
    SETUP("bri", "mfd", "2")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 2)
    FINISH()
})

TEST_CASE(brightness_mfd_126)
TEST_DEF({
    SETUP("bri", "mfd", "126")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 126)
    FINISH()
})

TEST_CASE(brightness_mfd_127)
TEST_DEF({
    SETUP("bri", "mfd", "127")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 127)
    FINISH()
})

TEST_CASE(brightness_mfd_128)
TEST_DEF({
    SETUP("bri", "mfd", "128")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 128)
    FINISH()
})

TEST_CASE(brightness_led_0)
TEST_DEF({
    SETUP("bri", "led", "0")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 0)
    FINISH()
})

TEST_CASE(brightness_led_1)
TEST_DEF({
    SETUP("bri", "led", "1")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 1)
    FINISH()
})

TEST_CASE(brightness_led_2)
TEST_DEF({
    SETUP("bri", "led", "2")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 2)
    FINISH()
})

TEST_CASE(brightness_led_126)
TEST_DEF({
    SETUP("bri", "led", "126")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 126)
    FINISH()
})

TEST_CASE(brightness_led_127)
TEST_DEF({
    SETUP("bri", "led", "127")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 127)
    FINISH()
})

TEST_CASE(brightness_led_128)
TEST_DEF({
    SETUP("bri", "led", "128")
    CONNECT_FUNCS()
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 128)
    FINISH()
})

/* }}} */

/* Indicator test cases {{{ */
TEST_CASE(indicator_blink_on)
TEST_DEF({
    SETUP("blink", "on")
    CONNECT_FUNCS()
    FUNC(set_blink, 1)
    EXPECT(set_blink, state, 1)
    FINISH()
})

TEST_CASE(indicator_blink_off)
TEST_DEF({
    SETUP("blink", "off")
    CONNECT_FUNCS()
    FUNC(set_blink, 1)
    EXPECT(set_blink, state, 0)
    FINISH()
})

TEST_CASE(indicator_shift_on)
TEST_DEF({
    SETUP("shift", "on")
    CONNECT_FUNCS()
    FUNC(set_shift, 1)
    EXPECT(set_shift, state, 1)
    FINISH()
})

TEST_CASE(indicator_shift_off)
TEST_DEF({
    SETUP("shift", "off")
    CONNECT_FUNCS()
    FUNC(set_shift, 1)
    EXPECT(set_shift, state, 0)
    FINISH()
})

/* }}} */

/* MFD text tests {{{ */
TEST_CASE(mfd_0_1)
TEST_DEF({
    SETUP("mfd", "0", "a")
    CONNECT_FUNCS()
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "a")
    EXPECT(set_text, length, 1)
    FINISH()
})

TEST_CASE(mfd_0_2)
TEST_DEF({
    SETUP("mfd", "0", "ab")
    CONNECT_FUNCS()
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "ab")
    EXPECT(set_text, length, 2)
    FINISH()
})

TEST_CASE(mfd_1_1)
TEST_DEF({
    SETUP("mfd", "1", "a")
    CONNECT_FUNCS()
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "a")
    EXPECT(set_text, length, 1)
    FINISH()
})

TEST_CASE(mfd_1_2)
TEST_DEF({
    SETUP("mfd", "1", "ab")
    CONNECT_FUNCS()
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "ab")
    EXPECT(set_text, length, 2)
    FINISH()
})

TEST_CASE(mfd_2_1)
TEST_DEF({
    SETUP("mfd", "2", "a")
    CONNECT_FUNCS()
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "a")
    EXPECT(set_text, length, 1)
    FINISH()
})

TEST_CASE(mfd_2_2)
TEST_DEF({
    SETUP("mfd", "2", "ab")
    CONNECT_FUNCS()
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "ab")
    EXPECT(set_text, length, 2)
    FINISH()
})

/* }}} */

/* Clock tests {{{ */
TEST_CASE(clock_local_12hr_ddmmyy)
TEST_DEF({
    SETUP("clock", "local", "12hr", "ddmmyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
})

TEST_CASE(clock_local_12hr_mmddyy)
TEST_DEF({
    SETUP("clock", "local", "12hr", "mmddyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
})

TEST_CASE(clock_local_12hr_yymmdd)
TEST_DEF({
    SETUP("clock", "local", "12hr", "yymmdd")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
})

TEST_CASE(clock_local_24hr_ddmmyy)
TEST_DEF({
    SETUP("clock", "local", "24hr", "ddmmyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
})

TEST_CASE(clock_local_24hr_mmddyy)
TEST_DEF({
    SETUP("clock", "local", "24hr", "mmddyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
})

TEST_CASE(clock_local_24hr_yymmdd)
TEST_DEF({
    SETUP("clock", "local", "24hr", "yymmdd")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 1)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
})

TEST_CASE(clock_gmt_12hr_ddmmyy)
TEST_DEF({
    SETUP("clock", "gmt", "12hr", "ddmmyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
})

TEST_CASE(clock_gmt_12hr_mmddyy)
TEST_DEF({
    SETUP("clock", "gmt", "12hr", "mmddyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
})

TEST_CASE(clock_gmt_12hr_yymmdd)
TEST_DEF({
    SETUP("clock", "gmt", "12hr", "yymmdd")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
})

TEST_CASE(clock_gmt_24hr_ddmmyy)
TEST_DEF({
    SETUP("clock", "gmt", "24hr", "ddmmyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
})

TEST_CASE(clock_gmt_24hr_mmddyy)
TEST_DEF({
    SETUP("clock", "gmt", "24hr", "mmddyy")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
})

TEST_CASE(clock_gmt_24hr_yymmdd)
TEST_DEF({
    SETUP("clock", "gmt", "24hr", "yymmdd")
    CONNECT_FUNCS()
    FUNC(set_clock, 1)
    EXPECT(set_clock, local, 0)
    EXPECT(set_clock, time, time(NULL))
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
})
/* }}} */

/* Clock offset tests {{{ */
TEST_CASE(offset_2__30_12hr)
TEST_DEF({
    SETUP("offset", "2", "-30", "12hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(offset_2__30_24hr)
TEST_DEF({
    SETUP("offset", "2", "-30", "24hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(offset_2_0_12hr)
TEST_DEF({
    SETUP("offset", "2", "0", "12hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(offset_2_0_24hr)
TEST_DEF({
    SETUP("offset", "2", "0", "24hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(offset_2_30_12hr)
TEST_DEF({
    SETUP("offset", "2", "30", "12hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(offset_2_30_24hr)
TEST_DEF({
    SETUP("offset", "2", "30", "24hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_2)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(offset_3__30_12hr)
TEST_DEF({
    SETUP("offset", "3", "-30", "12hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(offset_3__30_24hr)
TEST_DEF({
    SETUP("offset", "3", "-30", "24hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, -30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(offset_3_0_12hr)
TEST_DEF({
    SETUP("offset", "3", "0", "12hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(offset_3_0_24hr)
TEST_DEF({
    SETUP("offset", "3", "0", "24hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(offset_3_30_12hr)
TEST_DEF({
    SETUP("offset", "3", "30", "12hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(offset_3_30_24hr)
TEST_DEF({
    SETUP("offset", "3", "30", "24hr")
    CONNECT_FUNCS()
    FUNC(set_clock_timezone, 1)
    EXPECT(set_clock_timezone, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_timezone, offset, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_3)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

/* }}} */

/* Raw time tests {{{ */
TEST_CASE(raw_time_0_0_12hr)
TEST_DEF({
    SETUP("time", "0", "0", "12hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(raw_time_0_0_24hr)
TEST_DEF({
    SETUP("time", "0", "0", "24hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(raw_time_0_1_12hr)
TEST_DEF({
    SETUP("time", "0", "1", "12hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 1)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(raw_time_0_1_24hr)
TEST_DEF({
    SETUP("time", "0", "1", "24hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 1)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(raw_time_1_0_12hr)
TEST_DEF({
    SETUP("time", "1", "0", "12hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 1)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(raw_time_1_0_24hr)
TEST_DEF({
    SETUP("time", "1", "0", "24hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 1)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

TEST_CASE(raw_time_2_0_12hr)
TEST_DEF({
    SETUP("time", "2", "0", "12hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 2)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
})

TEST_CASE(raw_time_2_0_24hr)
TEST_DEF({
    SETUP("time", "2", "0", "24hr")
    CONNECT_FUNCS()
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 2)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
})

/* }}} */

/* Raw date tests {{{ */
TEST_CASE(raw_date_ddmmyy)
TEST_DEF({
    SETUP("date", "1", "2", "3", "ddmmyy")
    CONNECT_FUNCS()
    FUNC(set_date, 1)
    EXPECT(set_date, dd, 1)
    EXPECT(set_date, mm, 2)
    EXPECT(set_date, yy, 3)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_DDMMYY)
    FINISH()
})

TEST_CASE(raw_date_mmddyy)
TEST_DEF({
    SETUP("date", "1", "2", "3", "mmddyy")
    CONNECT_FUNCS()
    FUNC(set_date, 1)
    EXPECT(set_date, dd, 1)
    EXPECT(set_date, mm, 2)
    EXPECT(set_date, yy, 3)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_MMDDYY)
    FINISH()
})

TEST_CASE(raw_date_yymmdd)
TEST_DEF({
    SETUP("date", "1", "2", "3", "yymmdd")
    CONNECT_FUNCS()
    FUNC(set_date, 1)
    EXPECT(set_date, dd, 1)
    EXPECT(set_date, mm, 2)
    EXPECT(set_date, yy, 3)
    FUNC(set_date_format, 1)
    EXPECT(set_date_format, format, LIBX52_DATE_FORMAT_YYMMDD)
    FINISH()
})
/* }}} */

/* Vendor command tests {{{ */
TEST_CASE(vendor_command_hex)
TEST_DEF({
    SETUP("raw", "0x0123", "0x4567")
    CONNECT_FUNCS()
    FUNC(vendor_command, 1)
    EXPECT(vendor_command, index, 0x0123)
    EXPECT(vendor_command, value, 0x4567)
    FINISH()
})

TEST_CASE(vendor_command_oct)
TEST_DEF({
    SETUP("raw", "0123", "0456")
    CONNECT_FUNCS()
    FUNC(vendor_command, 1)
    EXPECT(vendor_command, index, 0123)
    EXPECT(vendor_command, value, 0456)
    FINISH()
})

TEST_CASE(vendor_command_dec)
TEST_DEF({
    SETUP("raw", "123", "456")
    CONNECT_FUNCS()
    FUNC(vendor_command, 1)
    EXPECT(vendor_command, index, 123)
    EXPECT(vendor_command, value, 456)
    FINISH()
})

/* }}} */

/* Command parsing tests {{{ */
TEST_CASE(parsing_no_args)
TEST_DEF({
    SETUP()
    FAIL()
})

TEST_CASE(parsing_invalid_command)
TEST_DEF({
    SETUP("foobar")
    FAIL()
})

TEST_CASE(parsing_insufficient_args)
TEST_DEF({
    SETUP("led")
    FAIL()
})

TEST_CASE(parsing_invalid_map_value)
TEST_DEF({
    SETUP("led", "fire", "oon")
    FAIL()
})

/* }}} */

/* libx52 error handling tests {{{ */
TEST_CASE(error_libx52_init)
TEST_DEF({
    SETUP("led", "fire", "on")
    expect_function_calls(libx52_init, 1);
    expect_function_calls(libx52_strerror, 1);
    will_return_count(libx52_init, LIBX52_ERROR_OUT_OF_MEMORY, 1);
    FAIL()
})

TEST_CASE(error_libx52_connect)
TEST_DEF({
    SETUP("led", "fire", "on")
    FUNC(init, 1)
    expect_function_calls(libx52_connect, 1);
    expect_function_calls(libx52_strerror, 1);
    will_return_count(libx52_connect, LIBX52_ERROR_NO_DEVICE, 1);
    FAIL()
})

TEST_CASE(error_libx52_unsupported)
TEST_DEF({
    SETUP("led", "fire", "off")
    CONNECT_FUNCS()
    expect_function_calls(libx52_set_led_state, 1);
    will_return_count(libx52_set_led_state, LIBX52_ERROR_NOT_SUPPORTED, 1);
    EXPECT(set_led_state, id, LIBX52_LED_FIRE)
    EXPECT(set_led_state, state, LIBX52_LED_STATE_OFF)
    expect_function_calls(libx52_strerror, 1);
    FAIL()
})

/* }}} */

#undef TEST_CASE
#undef TEST_DEF
