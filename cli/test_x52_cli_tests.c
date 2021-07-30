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

TEST_CASE(brightness_mfd_3)
#if TEST_DEF
{
    SETUP("bri", "mfd", "3")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 3)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_4)
#if TEST_DEF
{
    SETUP("bri", "mfd", "4")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 4)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_5)
#if TEST_DEF
{
    SETUP("bri", "mfd", "5")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 5)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_6)
#if TEST_DEF
{
    SETUP("bri", "mfd", "6")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 6)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_7)
#if TEST_DEF
{
    SETUP("bri", "mfd", "7")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 7)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_8)
#if TEST_DEF
{
    SETUP("bri", "mfd", "8")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 8)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_9)
#if TEST_DEF
{
    SETUP("bri", "mfd", "9")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 9)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_10)
#if TEST_DEF
{
    SETUP("bri", "mfd", "10")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 10)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_11)
#if TEST_DEF
{
    SETUP("bri", "mfd", "11")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 11)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_12)
#if TEST_DEF
{
    SETUP("bri", "mfd", "12")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 12)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_13)
#if TEST_DEF
{
    SETUP("bri", "mfd", "13")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 13)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_14)
#if TEST_DEF
{
    SETUP("bri", "mfd", "14")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 14)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_15)
#if TEST_DEF
{
    SETUP("bri", "mfd", "15")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 15)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_16)
#if TEST_DEF
{
    SETUP("bri", "mfd", "16")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 16)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_17)
#if TEST_DEF
{
    SETUP("bri", "mfd", "17")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 17)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_18)
#if TEST_DEF
{
    SETUP("bri", "mfd", "18")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 18)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_19)
#if TEST_DEF
{
    SETUP("bri", "mfd", "19")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 19)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_20)
#if TEST_DEF
{
    SETUP("bri", "mfd", "20")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 20)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_21)
#if TEST_DEF
{
    SETUP("bri", "mfd", "21")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 21)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_22)
#if TEST_DEF
{
    SETUP("bri", "mfd", "22")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 22)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_23)
#if TEST_DEF
{
    SETUP("bri", "mfd", "23")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 23)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_24)
#if TEST_DEF
{
    SETUP("bri", "mfd", "24")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 24)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_25)
#if TEST_DEF
{
    SETUP("bri", "mfd", "25")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 25)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_26)
#if TEST_DEF
{
    SETUP("bri", "mfd", "26")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 26)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_27)
#if TEST_DEF
{
    SETUP("bri", "mfd", "27")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 27)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_28)
#if TEST_DEF
{
    SETUP("bri", "mfd", "28")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 28)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_29)
#if TEST_DEF
{
    SETUP("bri", "mfd", "29")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 29)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_30)
#if TEST_DEF
{
    SETUP("bri", "mfd", "30")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 30)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_31)
#if TEST_DEF
{
    SETUP("bri", "mfd", "31")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 31)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_32)
#if TEST_DEF
{
    SETUP("bri", "mfd", "32")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 32)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_33)
#if TEST_DEF
{
    SETUP("bri", "mfd", "33")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 33)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_34)
#if TEST_DEF
{
    SETUP("bri", "mfd", "34")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 34)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_35)
#if TEST_DEF
{
    SETUP("bri", "mfd", "35")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 35)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_36)
#if TEST_DEF
{
    SETUP("bri", "mfd", "36")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 36)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_37)
#if TEST_DEF
{
    SETUP("bri", "mfd", "37")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 37)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_38)
#if TEST_DEF
{
    SETUP("bri", "mfd", "38")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 38)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_39)
#if TEST_DEF
{
    SETUP("bri", "mfd", "39")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 39)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_40)
#if TEST_DEF
{
    SETUP("bri", "mfd", "40")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 40)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_41)
#if TEST_DEF
{
    SETUP("bri", "mfd", "41")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 41)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_42)
#if TEST_DEF
{
    SETUP("bri", "mfd", "42")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 42)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_43)
#if TEST_DEF
{
    SETUP("bri", "mfd", "43")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 43)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_44)
#if TEST_DEF
{
    SETUP("bri", "mfd", "44")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 44)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_45)
#if TEST_DEF
{
    SETUP("bri", "mfd", "45")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 45)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_46)
#if TEST_DEF
{
    SETUP("bri", "mfd", "46")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 46)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_47)
#if TEST_DEF
{
    SETUP("bri", "mfd", "47")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 47)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_48)
#if TEST_DEF
{
    SETUP("bri", "mfd", "48")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 48)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_49)
#if TEST_DEF
{
    SETUP("bri", "mfd", "49")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 49)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_50)
#if TEST_DEF
{
    SETUP("bri", "mfd", "50")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 50)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_51)
#if TEST_DEF
{
    SETUP("bri", "mfd", "51")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 51)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_52)
#if TEST_DEF
{
    SETUP("bri", "mfd", "52")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 52)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_53)
#if TEST_DEF
{
    SETUP("bri", "mfd", "53")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 53)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_54)
#if TEST_DEF
{
    SETUP("bri", "mfd", "54")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 54)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_55)
#if TEST_DEF
{
    SETUP("bri", "mfd", "55")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 55)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_56)
#if TEST_DEF
{
    SETUP("bri", "mfd", "56")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 56)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_57)
#if TEST_DEF
{
    SETUP("bri", "mfd", "57")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 57)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_58)
#if TEST_DEF
{
    SETUP("bri", "mfd", "58")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 58)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_59)
#if TEST_DEF
{
    SETUP("bri", "mfd", "59")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 59)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_60)
#if TEST_DEF
{
    SETUP("bri", "mfd", "60")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 60)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_61)
#if TEST_DEF
{
    SETUP("bri", "mfd", "61")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 61)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_62)
#if TEST_DEF
{
    SETUP("bri", "mfd", "62")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 62)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_63)
#if TEST_DEF
{
    SETUP("bri", "mfd", "63")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 63)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_64)
#if TEST_DEF
{
    SETUP("bri", "mfd", "64")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 64)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_65)
#if TEST_DEF
{
    SETUP("bri", "mfd", "65")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 65)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_66)
#if TEST_DEF
{
    SETUP("bri", "mfd", "66")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 66)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_67)
#if TEST_DEF
{
    SETUP("bri", "mfd", "67")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 67)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_68)
#if TEST_DEF
{
    SETUP("bri", "mfd", "68")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 68)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_69)
#if TEST_DEF
{
    SETUP("bri", "mfd", "69")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 69)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_70)
#if TEST_DEF
{
    SETUP("bri", "mfd", "70")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 70)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_71)
#if TEST_DEF
{
    SETUP("bri", "mfd", "71")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 71)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_72)
#if TEST_DEF
{
    SETUP("bri", "mfd", "72")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 72)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_73)
#if TEST_DEF
{
    SETUP("bri", "mfd", "73")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 73)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_74)
#if TEST_DEF
{
    SETUP("bri", "mfd", "74")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 74)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_75)
#if TEST_DEF
{
    SETUP("bri", "mfd", "75")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 75)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_76)
#if TEST_DEF
{
    SETUP("bri", "mfd", "76")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 76)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_77)
#if TEST_DEF
{
    SETUP("bri", "mfd", "77")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 77)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_78)
#if TEST_DEF
{
    SETUP("bri", "mfd", "78")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 78)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_79)
#if TEST_DEF
{
    SETUP("bri", "mfd", "79")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 79)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_80)
#if TEST_DEF
{
    SETUP("bri", "mfd", "80")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 80)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_81)
#if TEST_DEF
{
    SETUP("bri", "mfd", "81")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 81)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_82)
#if TEST_DEF
{
    SETUP("bri", "mfd", "82")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 82)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_83)
#if TEST_DEF
{
    SETUP("bri", "mfd", "83")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 83)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_84)
#if TEST_DEF
{
    SETUP("bri", "mfd", "84")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 84)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_85)
#if TEST_DEF
{
    SETUP("bri", "mfd", "85")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 85)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_86)
#if TEST_DEF
{
    SETUP("bri", "mfd", "86")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 86)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_87)
#if TEST_DEF
{
    SETUP("bri", "mfd", "87")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 87)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_88)
#if TEST_DEF
{
    SETUP("bri", "mfd", "88")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 88)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_89)
#if TEST_DEF
{
    SETUP("bri", "mfd", "89")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 89)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_90)
#if TEST_DEF
{
    SETUP("bri", "mfd", "90")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 90)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_91)
#if TEST_DEF
{
    SETUP("bri", "mfd", "91")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 91)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_92)
#if TEST_DEF
{
    SETUP("bri", "mfd", "92")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 92)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_93)
#if TEST_DEF
{
    SETUP("bri", "mfd", "93")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 93)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_94)
#if TEST_DEF
{
    SETUP("bri", "mfd", "94")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 94)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_95)
#if TEST_DEF
{
    SETUP("bri", "mfd", "95")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 95)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_96)
#if TEST_DEF
{
    SETUP("bri", "mfd", "96")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 96)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_97)
#if TEST_DEF
{
    SETUP("bri", "mfd", "97")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 97)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_98)
#if TEST_DEF
{
    SETUP("bri", "mfd", "98")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 98)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_99)
#if TEST_DEF
{
    SETUP("bri", "mfd", "99")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 99)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_100)
#if TEST_DEF
{
    SETUP("bri", "mfd", "100")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 100)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_101)
#if TEST_DEF
{
    SETUP("bri", "mfd", "101")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 101)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_102)
#if TEST_DEF
{
    SETUP("bri", "mfd", "102")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 102)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_103)
#if TEST_DEF
{
    SETUP("bri", "mfd", "103")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 103)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_104)
#if TEST_DEF
{
    SETUP("bri", "mfd", "104")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 104)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_105)
#if TEST_DEF
{
    SETUP("bri", "mfd", "105")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 105)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_106)
#if TEST_DEF
{
    SETUP("bri", "mfd", "106")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 106)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_107)
#if TEST_DEF
{
    SETUP("bri", "mfd", "107")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 107)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_108)
#if TEST_DEF
{
    SETUP("bri", "mfd", "108")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 108)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_109)
#if TEST_DEF
{
    SETUP("bri", "mfd", "109")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 109)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_110)
#if TEST_DEF
{
    SETUP("bri", "mfd", "110")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 110)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_111)
#if TEST_DEF
{
    SETUP("bri", "mfd", "111")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 111)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_112)
#if TEST_DEF
{
    SETUP("bri", "mfd", "112")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 112)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_113)
#if TEST_DEF
{
    SETUP("bri", "mfd", "113")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 113)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_114)
#if TEST_DEF
{
    SETUP("bri", "mfd", "114")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 114)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_115)
#if TEST_DEF
{
    SETUP("bri", "mfd", "115")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 115)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_116)
#if TEST_DEF
{
    SETUP("bri", "mfd", "116")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 116)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_117)
#if TEST_DEF
{
    SETUP("bri", "mfd", "117")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 117)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_118)
#if TEST_DEF
{
    SETUP("bri", "mfd", "118")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 118)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_119)
#if TEST_DEF
{
    SETUP("bri", "mfd", "119")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 119)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_120)
#if TEST_DEF
{
    SETUP("bri", "mfd", "120")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 120)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_121)
#if TEST_DEF
{
    SETUP("bri", "mfd", "121")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 121)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_122)
#if TEST_DEF
{
    SETUP("bri", "mfd", "122")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 122)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_123)
#if TEST_DEF
{
    SETUP("bri", "mfd", "123")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 123)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_124)
#if TEST_DEF
{
    SETUP("bri", "mfd", "124")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 124)
    FINISH()
}
#endif

TEST_CASE(brightness_mfd_125)
#if TEST_DEF
{
    SETUP("bri", "mfd", "125")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 1)
    EXPECT(set_brightness, brightness, 125)
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

TEST_CASE(brightness_led_3)
#if TEST_DEF
{
    SETUP("bri", "led", "3")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 3)
    FINISH()
}
#endif

TEST_CASE(brightness_led_4)
#if TEST_DEF
{
    SETUP("bri", "led", "4")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 4)
    FINISH()
}
#endif

TEST_CASE(brightness_led_5)
#if TEST_DEF
{
    SETUP("bri", "led", "5")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 5)
    FINISH()
}
#endif

TEST_CASE(brightness_led_6)
#if TEST_DEF
{
    SETUP("bri", "led", "6")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 6)
    FINISH()
}
#endif

TEST_CASE(brightness_led_7)
#if TEST_DEF
{
    SETUP("bri", "led", "7")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 7)
    FINISH()
}
#endif

TEST_CASE(brightness_led_8)
#if TEST_DEF
{
    SETUP("bri", "led", "8")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 8)
    FINISH()
}
#endif

TEST_CASE(brightness_led_9)
#if TEST_DEF
{
    SETUP("bri", "led", "9")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 9)
    FINISH()
}
#endif

TEST_CASE(brightness_led_10)
#if TEST_DEF
{
    SETUP("bri", "led", "10")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 10)
    FINISH()
}
#endif

TEST_CASE(brightness_led_11)
#if TEST_DEF
{
    SETUP("bri", "led", "11")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 11)
    FINISH()
}
#endif

TEST_CASE(brightness_led_12)
#if TEST_DEF
{
    SETUP("bri", "led", "12")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 12)
    FINISH()
}
#endif

TEST_CASE(brightness_led_13)
#if TEST_DEF
{
    SETUP("bri", "led", "13")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 13)
    FINISH()
}
#endif

TEST_CASE(brightness_led_14)
#if TEST_DEF
{
    SETUP("bri", "led", "14")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 14)
    FINISH()
}
#endif

TEST_CASE(brightness_led_15)
#if TEST_DEF
{
    SETUP("bri", "led", "15")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 15)
    FINISH()
}
#endif

TEST_CASE(brightness_led_16)
#if TEST_DEF
{
    SETUP("bri", "led", "16")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 16)
    FINISH()
}
#endif

TEST_CASE(brightness_led_17)
#if TEST_DEF
{
    SETUP("bri", "led", "17")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 17)
    FINISH()
}
#endif

TEST_CASE(brightness_led_18)
#if TEST_DEF
{
    SETUP("bri", "led", "18")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 18)
    FINISH()
}
#endif

TEST_CASE(brightness_led_19)
#if TEST_DEF
{
    SETUP("bri", "led", "19")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 19)
    FINISH()
}
#endif

TEST_CASE(brightness_led_20)
#if TEST_DEF
{
    SETUP("bri", "led", "20")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 20)
    FINISH()
}
#endif

TEST_CASE(brightness_led_21)
#if TEST_DEF
{
    SETUP("bri", "led", "21")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 21)
    FINISH()
}
#endif

TEST_CASE(brightness_led_22)
#if TEST_DEF
{
    SETUP("bri", "led", "22")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 22)
    FINISH()
}
#endif

TEST_CASE(brightness_led_23)
#if TEST_DEF
{
    SETUP("bri", "led", "23")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 23)
    FINISH()
}
#endif

TEST_CASE(brightness_led_24)
#if TEST_DEF
{
    SETUP("bri", "led", "24")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 24)
    FINISH()
}
#endif

TEST_CASE(brightness_led_25)
#if TEST_DEF
{
    SETUP("bri", "led", "25")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 25)
    FINISH()
}
#endif

TEST_CASE(brightness_led_26)
#if TEST_DEF
{
    SETUP("bri", "led", "26")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 26)
    FINISH()
}
#endif

TEST_CASE(brightness_led_27)
#if TEST_DEF
{
    SETUP("bri", "led", "27")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 27)
    FINISH()
}
#endif

TEST_CASE(brightness_led_28)
#if TEST_DEF
{
    SETUP("bri", "led", "28")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 28)
    FINISH()
}
#endif

TEST_CASE(brightness_led_29)
#if TEST_DEF
{
    SETUP("bri", "led", "29")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 29)
    FINISH()
}
#endif

TEST_CASE(brightness_led_30)
#if TEST_DEF
{
    SETUP("bri", "led", "30")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 30)
    FINISH()
}
#endif

TEST_CASE(brightness_led_31)
#if TEST_DEF
{
    SETUP("bri", "led", "31")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 31)
    FINISH()
}
#endif

TEST_CASE(brightness_led_32)
#if TEST_DEF
{
    SETUP("bri", "led", "32")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 32)
    FINISH()
}
#endif

TEST_CASE(brightness_led_33)
#if TEST_DEF
{
    SETUP("bri", "led", "33")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 33)
    FINISH()
}
#endif

TEST_CASE(brightness_led_34)
#if TEST_DEF
{
    SETUP("bri", "led", "34")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 34)
    FINISH()
}
#endif

TEST_CASE(brightness_led_35)
#if TEST_DEF
{
    SETUP("bri", "led", "35")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 35)
    FINISH()
}
#endif

TEST_CASE(brightness_led_36)
#if TEST_DEF
{
    SETUP("bri", "led", "36")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 36)
    FINISH()
}
#endif

TEST_CASE(brightness_led_37)
#if TEST_DEF
{
    SETUP("bri", "led", "37")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 37)
    FINISH()
}
#endif

TEST_CASE(brightness_led_38)
#if TEST_DEF
{
    SETUP("bri", "led", "38")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 38)
    FINISH()
}
#endif

TEST_CASE(brightness_led_39)
#if TEST_DEF
{
    SETUP("bri", "led", "39")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 39)
    FINISH()
}
#endif

TEST_CASE(brightness_led_40)
#if TEST_DEF
{
    SETUP("bri", "led", "40")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 40)
    FINISH()
}
#endif

TEST_CASE(brightness_led_41)
#if TEST_DEF
{
    SETUP("bri", "led", "41")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 41)
    FINISH()
}
#endif

TEST_CASE(brightness_led_42)
#if TEST_DEF
{
    SETUP("bri", "led", "42")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 42)
    FINISH()
}
#endif

TEST_CASE(brightness_led_43)
#if TEST_DEF
{
    SETUP("bri", "led", "43")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 43)
    FINISH()
}
#endif

TEST_CASE(brightness_led_44)
#if TEST_DEF
{
    SETUP("bri", "led", "44")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 44)
    FINISH()
}
#endif

TEST_CASE(brightness_led_45)
#if TEST_DEF
{
    SETUP("bri", "led", "45")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 45)
    FINISH()
}
#endif

TEST_CASE(brightness_led_46)
#if TEST_DEF
{
    SETUP("bri", "led", "46")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 46)
    FINISH()
}
#endif

TEST_CASE(brightness_led_47)
#if TEST_DEF
{
    SETUP("bri", "led", "47")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 47)
    FINISH()
}
#endif

TEST_CASE(brightness_led_48)
#if TEST_DEF
{
    SETUP("bri", "led", "48")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 48)
    FINISH()
}
#endif

TEST_CASE(brightness_led_49)
#if TEST_DEF
{
    SETUP("bri", "led", "49")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 49)
    FINISH()
}
#endif

TEST_CASE(brightness_led_50)
#if TEST_DEF
{
    SETUP("bri", "led", "50")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 50)
    FINISH()
}
#endif

TEST_CASE(brightness_led_51)
#if TEST_DEF
{
    SETUP("bri", "led", "51")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 51)
    FINISH()
}
#endif

TEST_CASE(brightness_led_52)
#if TEST_DEF
{
    SETUP("bri", "led", "52")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 52)
    FINISH()
}
#endif

TEST_CASE(brightness_led_53)
#if TEST_DEF
{
    SETUP("bri", "led", "53")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 53)
    FINISH()
}
#endif

TEST_CASE(brightness_led_54)
#if TEST_DEF
{
    SETUP("bri", "led", "54")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 54)
    FINISH()
}
#endif

TEST_CASE(brightness_led_55)
#if TEST_DEF
{
    SETUP("bri", "led", "55")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 55)
    FINISH()
}
#endif

TEST_CASE(brightness_led_56)
#if TEST_DEF
{
    SETUP("bri", "led", "56")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 56)
    FINISH()
}
#endif

TEST_CASE(brightness_led_57)
#if TEST_DEF
{
    SETUP("bri", "led", "57")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 57)
    FINISH()
}
#endif

TEST_CASE(brightness_led_58)
#if TEST_DEF
{
    SETUP("bri", "led", "58")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 58)
    FINISH()
}
#endif

TEST_CASE(brightness_led_59)
#if TEST_DEF
{
    SETUP("bri", "led", "59")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 59)
    FINISH()
}
#endif

TEST_CASE(brightness_led_60)
#if TEST_DEF
{
    SETUP("bri", "led", "60")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 60)
    FINISH()
}
#endif

TEST_CASE(brightness_led_61)
#if TEST_DEF
{
    SETUP("bri", "led", "61")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 61)
    FINISH()
}
#endif

TEST_CASE(brightness_led_62)
#if TEST_DEF
{
    SETUP("bri", "led", "62")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 62)
    FINISH()
}
#endif

TEST_CASE(brightness_led_63)
#if TEST_DEF
{
    SETUP("bri", "led", "63")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 63)
    FINISH()
}
#endif

TEST_CASE(brightness_led_64)
#if TEST_DEF
{
    SETUP("bri", "led", "64")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 64)
    FINISH()
}
#endif

TEST_CASE(brightness_led_65)
#if TEST_DEF
{
    SETUP("bri", "led", "65")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 65)
    FINISH()
}
#endif

TEST_CASE(brightness_led_66)
#if TEST_DEF
{
    SETUP("bri", "led", "66")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 66)
    FINISH()
}
#endif

TEST_CASE(brightness_led_67)
#if TEST_DEF
{
    SETUP("bri", "led", "67")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 67)
    FINISH()
}
#endif

TEST_CASE(brightness_led_68)
#if TEST_DEF
{
    SETUP("bri", "led", "68")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 68)
    FINISH()
}
#endif

TEST_CASE(brightness_led_69)
#if TEST_DEF
{
    SETUP("bri", "led", "69")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 69)
    FINISH()
}
#endif

TEST_CASE(brightness_led_70)
#if TEST_DEF
{
    SETUP("bri", "led", "70")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 70)
    FINISH()
}
#endif

TEST_CASE(brightness_led_71)
#if TEST_DEF
{
    SETUP("bri", "led", "71")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 71)
    FINISH()
}
#endif

TEST_CASE(brightness_led_72)
#if TEST_DEF
{
    SETUP("bri", "led", "72")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 72)
    FINISH()
}
#endif

TEST_CASE(brightness_led_73)
#if TEST_DEF
{
    SETUP("bri", "led", "73")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 73)
    FINISH()
}
#endif

TEST_CASE(brightness_led_74)
#if TEST_DEF
{
    SETUP("bri", "led", "74")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 74)
    FINISH()
}
#endif

TEST_CASE(brightness_led_75)
#if TEST_DEF
{
    SETUP("bri", "led", "75")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 75)
    FINISH()
}
#endif

TEST_CASE(brightness_led_76)
#if TEST_DEF
{
    SETUP("bri", "led", "76")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 76)
    FINISH()
}
#endif

TEST_CASE(brightness_led_77)
#if TEST_DEF
{
    SETUP("bri", "led", "77")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 77)
    FINISH()
}
#endif

TEST_CASE(brightness_led_78)
#if TEST_DEF
{
    SETUP("bri", "led", "78")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 78)
    FINISH()
}
#endif

TEST_CASE(brightness_led_79)
#if TEST_DEF
{
    SETUP("bri", "led", "79")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 79)
    FINISH()
}
#endif

TEST_CASE(brightness_led_80)
#if TEST_DEF
{
    SETUP("bri", "led", "80")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 80)
    FINISH()
}
#endif

TEST_CASE(brightness_led_81)
#if TEST_DEF
{
    SETUP("bri", "led", "81")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 81)
    FINISH()
}
#endif

TEST_CASE(brightness_led_82)
#if TEST_DEF
{
    SETUP("bri", "led", "82")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 82)
    FINISH()
}
#endif

TEST_CASE(brightness_led_83)
#if TEST_DEF
{
    SETUP("bri", "led", "83")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 83)
    FINISH()
}
#endif

TEST_CASE(brightness_led_84)
#if TEST_DEF
{
    SETUP("bri", "led", "84")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 84)
    FINISH()
}
#endif

TEST_CASE(brightness_led_85)
#if TEST_DEF
{
    SETUP("bri", "led", "85")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 85)
    FINISH()
}
#endif

TEST_CASE(brightness_led_86)
#if TEST_DEF
{
    SETUP("bri", "led", "86")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 86)
    FINISH()
}
#endif

TEST_CASE(brightness_led_87)
#if TEST_DEF
{
    SETUP("bri", "led", "87")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 87)
    FINISH()
}
#endif

TEST_CASE(brightness_led_88)
#if TEST_DEF
{
    SETUP("bri", "led", "88")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 88)
    FINISH()
}
#endif

TEST_CASE(brightness_led_89)
#if TEST_DEF
{
    SETUP("bri", "led", "89")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 89)
    FINISH()
}
#endif

TEST_CASE(brightness_led_90)
#if TEST_DEF
{
    SETUP("bri", "led", "90")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 90)
    FINISH()
}
#endif

TEST_CASE(brightness_led_91)
#if TEST_DEF
{
    SETUP("bri", "led", "91")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 91)
    FINISH()
}
#endif

TEST_CASE(brightness_led_92)
#if TEST_DEF
{
    SETUP("bri", "led", "92")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 92)
    FINISH()
}
#endif

TEST_CASE(brightness_led_93)
#if TEST_DEF
{
    SETUP("bri", "led", "93")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 93)
    FINISH()
}
#endif

TEST_CASE(brightness_led_94)
#if TEST_DEF
{
    SETUP("bri", "led", "94")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 94)
    FINISH()
}
#endif

TEST_CASE(brightness_led_95)
#if TEST_DEF
{
    SETUP("bri", "led", "95")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 95)
    FINISH()
}
#endif

TEST_CASE(brightness_led_96)
#if TEST_DEF
{
    SETUP("bri", "led", "96")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 96)
    FINISH()
}
#endif

TEST_CASE(brightness_led_97)
#if TEST_DEF
{
    SETUP("bri", "led", "97")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 97)
    FINISH()
}
#endif

TEST_CASE(brightness_led_98)
#if TEST_DEF
{
    SETUP("bri", "led", "98")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 98)
    FINISH()
}
#endif

TEST_CASE(brightness_led_99)
#if TEST_DEF
{
    SETUP("bri", "led", "99")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 99)
    FINISH()
}
#endif

TEST_CASE(brightness_led_100)
#if TEST_DEF
{
    SETUP("bri", "led", "100")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 100)
    FINISH()
}
#endif

TEST_CASE(brightness_led_101)
#if TEST_DEF
{
    SETUP("bri", "led", "101")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 101)
    FINISH()
}
#endif

TEST_CASE(brightness_led_102)
#if TEST_DEF
{
    SETUP("bri", "led", "102")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 102)
    FINISH()
}
#endif

TEST_CASE(brightness_led_103)
#if TEST_DEF
{
    SETUP("bri", "led", "103")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 103)
    FINISH()
}
#endif

TEST_CASE(brightness_led_104)
#if TEST_DEF
{
    SETUP("bri", "led", "104")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 104)
    FINISH()
}
#endif

TEST_CASE(brightness_led_105)
#if TEST_DEF
{
    SETUP("bri", "led", "105")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 105)
    FINISH()
}
#endif

TEST_CASE(brightness_led_106)
#if TEST_DEF
{
    SETUP("bri", "led", "106")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 106)
    FINISH()
}
#endif

TEST_CASE(brightness_led_107)
#if TEST_DEF
{
    SETUP("bri", "led", "107")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 107)
    FINISH()
}
#endif

TEST_CASE(brightness_led_108)
#if TEST_DEF
{
    SETUP("bri", "led", "108")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 108)
    FINISH()
}
#endif

TEST_CASE(brightness_led_109)
#if TEST_DEF
{
    SETUP("bri", "led", "109")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 109)
    FINISH()
}
#endif

TEST_CASE(brightness_led_110)
#if TEST_DEF
{
    SETUP("bri", "led", "110")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 110)
    FINISH()
}
#endif

TEST_CASE(brightness_led_111)
#if TEST_DEF
{
    SETUP("bri", "led", "111")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 111)
    FINISH()
}
#endif

TEST_CASE(brightness_led_112)
#if TEST_DEF
{
    SETUP("bri", "led", "112")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 112)
    FINISH()
}
#endif

TEST_CASE(brightness_led_113)
#if TEST_DEF
{
    SETUP("bri", "led", "113")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 113)
    FINISH()
}
#endif

TEST_CASE(brightness_led_114)
#if TEST_DEF
{
    SETUP("bri", "led", "114")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 114)
    FINISH()
}
#endif

TEST_CASE(brightness_led_115)
#if TEST_DEF
{
    SETUP("bri", "led", "115")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 115)
    FINISH()
}
#endif

TEST_CASE(brightness_led_116)
#if TEST_DEF
{
    SETUP("bri", "led", "116")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 116)
    FINISH()
}
#endif

TEST_CASE(brightness_led_117)
#if TEST_DEF
{
    SETUP("bri", "led", "117")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 117)
    FINISH()
}
#endif

TEST_CASE(brightness_led_118)
#if TEST_DEF
{
    SETUP("bri", "led", "118")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 118)
    FINISH()
}
#endif

TEST_CASE(brightness_led_119)
#if TEST_DEF
{
    SETUP("bri", "led", "119")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 119)
    FINISH()
}
#endif

TEST_CASE(brightness_led_120)
#if TEST_DEF
{
    SETUP("bri", "led", "120")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 120)
    FINISH()
}
#endif

TEST_CASE(brightness_led_121)
#if TEST_DEF
{
    SETUP("bri", "led", "121")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 121)
    FINISH()
}
#endif

TEST_CASE(brightness_led_122)
#if TEST_DEF
{
    SETUP("bri", "led", "122")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 122)
    FINISH()
}
#endif

TEST_CASE(brightness_led_123)
#if TEST_DEF
{
    SETUP("bri", "led", "123")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 123)
    FINISH()
}
#endif

TEST_CASE(brightness_led_124)
#if TEST_DEF
{
    SETUP("bri", "led", "124")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 124)
    FINISH()
}
#endif

TEST_CASE(brightness_led_125)
#if TEST_DEF
{
    SETUP("bri", "led", "125")
    FUNC(set_brightness, 1)
    EXPECT(set_brightness, mfd, 0)
    EXPECT(set_brightness, brightness, 125)
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

TEST_CASE(mfd_0_3)
#if TEST_DEF
{
    SETUP("mfd", "0", "abc")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abc")
    EXPECT(set_text, length, 3)
    FINISH()
}
#endif

TEST_CASE(mfd_0_4)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcd")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcd")
    EXPECT(set_text, length, 4)
    FINISH()
}
#endif

TEST_CASE(mfd_0_5)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcde")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcde")
    EXPECT(set_text, length, 5)
    FINISH()
}
#endif

TEST_CASE(mfd_0_6)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdef")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdef")
    EXPECT(set_text, length, 6)
    FINISH()
}
#endif

TEST_CASE(mfd_0_7)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefg")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefg")
    EXPECT(set_text, length, 7)
    FINISH()
}
#endif

TEST_CASE(mfd_0_8)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefgh")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefgh")
    EXPECT(set_text, length, 8)
    FINISH()
}
#endif

TEST_CASE(mfd_0_9)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghi")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghi")
    EXPECT(set_text, length, 9)
    FINISH()
}
#endif

TEST_CASE(mfd_0_10)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghij")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghij")
    EXPECT(set_text, length, 10)
    FINISH()
}
#endif

TEST_CASE(mfd_0_11)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijk")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijk")
    EXPECT(set_text, length, 11)
    FINISH()
}
#endif

TEST_CASE(mfd_0_12)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijkl")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijkl")
    EXPECT(set_text, length, 12)
    FINISH()
}
#endif

TEST_CASE(mfd_0_13)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijklm")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijklm")
    EXPECT(set_text, length, 13)
    FINISH()
}
#endif

TEST_CASE(mfd_0_14)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijklmn")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijklmn")
    EXPECT(set_text, length, 14)
    FINISH()
}
#endif

TEST_CASE(mfd_0_15)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijklmno")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijklmno")
    EXPECT(set_text, length, 15)
    FINISH()
}
#endif

TEST_CASE(mfd_0_16)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijklmnop")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijklmnop")
    EXPECT(set_text, length, 16)
    FINISH()
}
#endif

TEST_CASE(mfd_0_17)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijklmnopq")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijklmnopq")
    EXPECT(set_text, length, 17)
    FINISH()
}
#endif

TEST_CASE(mfd_0_18)
#if TEST_DEF
{
    SETUP("mfd", "0", "abcdefghijklmnopqr")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 0)
    EXPECT_STRING(set_text, text, "abcdefghijklmnopqr")
    EXPECT(set_text, length, 18)
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

TEST_CASE(mfd_1_3)
#if TEST_DEF
{
    SETUP("mfd", "1", "abc")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abc")
    EXPECT(set_text, length, 3)
    FINISH()
}
#endif

TEST_CASE(mfd_1_4)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcd")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcd")
    EXPECT(set_text, length, 4)
    FINISH()
}
#endif

TEST_CASE(mfd_1_5)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcde")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcde")
    EXPECT(set_text, length, 5)
    FINISH()
}
#endif

TEST_CASE(mfd_1_6)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdef")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdef")
    EXPECT(set_text, length, 6)
    FINISH()
}
#endif

TEST_CASE(mfd_1_7)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefg")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefg")
    EXPECT(set_text, length, 7)
    FINISH()
}
#endif

TEST_CASE(mfd_1_8)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefgh")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefgh")
    EXPECT(set_text, length, 8)
    FINISH()
}
#endif

TEST_CASE(mfd_1_9)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghi")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghi")
    EXPECT(set_text, length, 9)
    FINISH()
}
#endif

TEST_CASE(mfd_1_10)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghij")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghij")
    EXPECT(set_text, length, 10)
    FINISH()
}
#endif

TEST_CASE(mfd_1_11)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijk")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijk")
    EXPECT(set_text, length, 11)
    FINISH()
}
#endif

TEST_CASE(mfd_1_12)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijkl")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijkl")
    EXPECT(set_text, length, 12)
    FINISH()
}
#endif

TEST_CASE(mfd_1_13)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijklm")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijklm")
    EXPECT(set_text, length, 13)
    FINISH()
}
#endif

TEST_CASE(mfd_1_14)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijklmn")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijklmn")
    EXPECT(set_text, length, 14)
    FINISH()
}
#endif

TEST_CASE(mfd_1_15)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijklmno")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijklmno")
    EXPECT(set_text, length, 15)
    FINISH()
}
#endif

TEST_CASE(mfd_1_16)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijklmnop")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijklmnop")
    EXPECT(set_text, length, 16)
    FINISH()
}
#endif

TEST_CASE(mfd_1_17)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijklmnopq")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijklmnopq")
    EXPECT(set_text, length, 17)
    FINISH()
}
#endif

TEST_CASE(mfd_1_18)
#if TEST_DEF
{
    SETUP("mfd", "1", "abcdefghijklmnopqr")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 1)
    EXPECT_STRING(set_text, text, "abcdefghijklmnopqr")
    EXPECT(set_text, length, 18)
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

TEST_CASE(mfd_2_3)
#if TEST_DEF
{
    SETUP("mfd", "2", "abc")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abc")
    EXPECT(set_text, length, 3)
    FINISH()
}
#endif

TEST_CASE(mfd_2_4)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcd")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcd")
    EXPECT(set_text, length, 4)
    FINISH()
}
#endif

TEST_CASE(mfd_2_5)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcde")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcde")
    EXPECT(set_text, length, 5)
    FINISH()
}
#endif

TEST_CASE(mfd_2_6)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdef")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdef")
    EXPECT(set_text, length, 6)
    FINISH()
}
#endif

TEST_CASE(mfd_2_7)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefg")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefg")
    EXPECT(set_text, length, 7)
    FINISH()
}
#endif

TEST_CASE(mfd_2_8)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefgh")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefgh")
    EXPECT(set_text, length, 8)
    FINISH()
}
#endif

TEST_CASE(mfd_2_9)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghi")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghi")
    EXPECT(set_text, length, 9)
    FINISH()
}
#endif

TEST_CASE(mfd_2_10)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghij")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghij")
    EXPECT(set_text, length, 10)
    FINISH()
}
#endif

TEST_CASE(mfd_2_11)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijk")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijk")
    EXPECT(set_text, length, 11)
    FINISH()
}
#endif

TEST_CASE(mfd_2_12)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijkl")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijkl")
    EXPECT(set_text, length, 12)
    FINISH()
}
#endif

TEST_CASE(mfd_2_13)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijklm")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijklm")
    EXPECT(set_text, length, 13)
    FINISH()
}
#endif

TEST_CASE(mfd_2_14)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijklmn")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijklmn")
    EXPECT(set_text, length, 14)
    FINISH()
}
#endif

TEST_CASE(mfd_2_15)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijklmno")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijklmno")
    EXPECT(set_text, length, 15)
    FINISH()
}
#endif

TEST_CASE(mfd_2_16)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijklmnop")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijklmnop")
    EXPECT(set_text, length, 16)
    FINISH()
}
#endif

TEST_CASE(mfd_2_17)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijklmnopq")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijklmnopq")
    EXPECT(set_text, length, 17)
    FINISH()
}
#endif

TEST_CASE(mfd_2_18)
#if TEST_DEF
{
    SETUP("mfd", "2", "abcdefghijklmnopqr")
    FUNC(set_text, 1)
    EXPECT(set_text, line, 2)
    EXPECT_STRING(set_text, text, "abcdefghijklmnopqr")
    EXPECT(set_text, length, 18)
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

TEST_CASE(raw_time_0_2_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "2", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 2)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_2_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "2", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 2)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_3_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "3", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 3)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_3_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "3", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 3)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_4_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "4", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 4)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_4_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "4", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 4)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_5_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "5", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 5)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_5_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "5", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 5)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_6_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "6", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 6)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_6_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "6", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 6)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_7_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "7", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 7)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_7_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "7", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 7)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_8_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "8", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 8)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_8_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "8", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 8)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_9_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "9", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 9)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_9_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "9", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 9)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_10_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "10", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 10)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_10_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "10", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 10)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_11_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "11", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 11)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_11_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "11", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 11)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_12_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "12", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 12)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_12_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "12", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 12)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_13_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "13", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 13)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_13_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "13", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 13)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_14_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "14", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 14)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_14_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "14", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 14)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_15_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "15", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 15)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_15_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "15", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 15)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_16_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "16", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 16)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_16_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "16", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 16)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_17_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "17", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 17)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_17_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "17", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 17)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_18_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "18", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 18)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_18_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "18", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 18)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_19_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "19", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 19)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_19_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "19", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 19)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_20_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "20", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 20)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_20_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "20", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 20)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_21_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "21", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 21)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_21_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "21", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 21)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_22_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "22", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 22)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_22_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "22", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 22)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_23_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "23", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 23)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_23_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "23", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 23)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_24_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "24", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 24)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_24_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "24", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 24)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_25_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "25", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 25)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_25_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "25", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 25)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_26_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "26", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 26)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_26_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "26", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 26)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_27_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "27", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 27)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_27_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "27", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 27)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_28_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "28", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 28)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_28_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "28", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 28)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_29_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "29", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 29)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_29_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "29", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 29)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_30_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "30", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_30_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "30", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 30)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_31_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "31", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 31)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_31_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "31", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 31)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_32_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "32", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 32)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_32_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "32", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 32)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_33_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "33", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 33)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_33_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "33", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 33)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_34_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "34", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 34)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_34_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "34", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 34)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_35_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "35", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 35)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_35_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "35", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 35)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_36_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "36", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 36)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_36_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "36", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 36)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_37_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "37", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 37)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_37_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "37", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 37)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_38_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "38", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 38)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_38_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "38", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 38)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_39_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "39", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 39)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_39_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "39", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 39)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_40_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "40", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 40)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_40_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "40", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 40)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_41_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "41", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 41)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_41_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "41", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 41)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_42_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "42", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 42)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_42_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "42", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 42)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_43_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "43", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 43)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_43_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "43", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 43)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_44_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "44", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 44)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_44_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "44", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 44)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_45_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "45", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 45)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_45_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "45", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 45)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_46_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "46", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 46)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_46_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "46", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 46)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_47_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "47", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 47)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_47_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "47", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 47)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_48_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "48", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 48)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_48_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "48", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 48)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_49_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "49", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 49)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_49_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "49", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 49)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_50_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "50", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 50)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_50_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "50", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 50)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_51_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "51", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 51)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_51_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "51", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 51)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_52_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "52", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 52)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_52_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "52", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 52)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_53_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "53", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 53)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_53_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "53", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 53)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_54_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "54", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 54)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_54_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "54", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 54)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_55_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "55", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 55)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_55_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "55", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 55)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_56_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "56", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 56)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_56_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "56", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 56)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_57_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "57", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 57)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_57_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "57", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 57)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_58_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "58", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 58)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_58_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "58", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 58)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_59_12hr)
#if TEST_DEF
{
    SETUP("time", "0", "59", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 59)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_0_59_24hr)
#if TEST_DEF
{
    SETUP("time", "0", "59", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 0)
    EXPECT(set_time, minute, 59)
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

TEST_CASE(raw_time_3_0_12hr)
#if TEST_DEF
{
    SETUP("time", "3", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 3)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_3_0_24hr)
#if TEST_DEF
{
    SETUP("time", "3", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 3)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_4_0_12hr)
#if TEST_DEF
{
    SETUP("time", "4", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 4)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_4_0_24hr)
#if TEST_DEF
{
    SETUP("time", "4", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 4)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_5_0_12hr)
#if TEST_DEF
{
    SETUP("time", "5", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 5)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_5_0_24hr)
#if TEST_DEF
{
    SETUP("time", "5", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 5)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_6_0_12hr)
#if TEST_DEF
{
    SETUP("time", "6", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 6)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_6_0_24hr)
#if TEST_DEF
{
    SETUP("time", "6", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 6)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_7_0_12hr)
#if TEST_DEF
{
    SETUP("time", "7", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 7)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_7_0_24hr)
#if TEST_DEF
{
    SETUP("time", "7", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 7)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_8_0_12hr)
#if TEST_DEF
{
    SETUP("time", "8", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 8)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_8_0_24hr)
#if TEST_DEF
{
    SETUP("time", "8", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 8)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_9_0_12hr)
#if TEST_DEF
{
    SETUP("time", "9", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 9)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_9_0_24hr)
#if TEST_DEF
{
    SETUP("time", "9", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 9)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_10_0_12hr)
#if TEST_DEF
{
    SETUP("time", "10", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 10)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_10_0_24hr)
#if TEST_DEF
{
    SETUP("time", "10", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 10)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_11_0_12hr)
#if TEST_DEF
{
    SETUP("time", "11", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 11)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_11_0_24hr)
#if TEST_DEF
{
    SETUP("time", "11", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 11)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_12_0_12hr)
#if TEST_DEF
{
    SETUP("time", "12", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 12)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_12_0_24hr)
#if TEST_DEF
{
    SETUP("time", "12", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 12)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_13_0_12hr)
#if TEST_DEF
{
    SETUP("time", "13", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 13)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_13_0_24hr)
#if TEST_DEF
{
    SETUP("time", "13", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 13)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_14_0_12hr)
#if TEST_DEF
{
    SETUP("time", "14", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 14)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_14_0_24hr)
#if TEST_DEF
{
    SETUP("time", "14", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 14)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_15_0_12hr)
#if TEST_DEF
{
    SETUP("time", "15", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 15)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_15_0_24hr)
#if TEST_DEF
{
    SETUP("time", "15", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 15)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_16_0_12hr)
#if TEST_DEF
{
    SETUP("time", "16", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 16)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_16_0_24hr)
#if TEST_DEF
{
    SETUP("time", "16", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 16)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_17_0_12hr)
#if TEST_DEF
{
    SETUP("time", "17", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 17)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_17_0_24hr)
#if TEST_DEF
{
    SETUP("time", "17", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 17)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_18_0_12hr)
#if TEST_DEF
{
    SETUP("time", "18", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 18)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_18_0_24hr)
#if TEST_DEF
{
    SETUP("time", "18", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 18)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_19_0_12hr)
#if TEST_DEF
{
    SETUP("time", "19", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 19)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_19_0_24hr)
#if TEST_DEF
{
    SETUP("time", "19", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 19)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_20_0_12hr)
#if TEST_DEF
{
    SETUP("time", "20", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 20)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_20_0_24hr)
#if TEST_DEF
{
    SETUP("time", "20", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 20)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_21_0_12hr)
#if TEST_DEF
{
    SETUP("time", "21", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 21)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_21_0_24hr)
#if TEST_DEF
{
    SETUP("time", "21", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 21)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_22_0_12hr)
#if TEST_DEF
{
    SETUP("time", "22", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 22)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_22_0_24hr)
#if TEST_DEF
{
    SETUP("time", "22", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 22)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_24HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_23_0_12hr)
#if TEST_DEF
{
    SETUP("time", "23", "0", "12hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 23)
    EXPECT(set_time, minute, 0)
    FUNC(set_clock_format, 1)
    EXPECT(set_clock_format, clock, LIBX52_CLOCK_1)
    EXPECT(set_clock_format, format, LIBX52_CLOCK_FORMAT_12HR)
    FINISH()
}
#endif

TEST_CASE(raw_time_23_0_24hr)
#if TEST_DEF
{
    SETUP("time", "23", "0", "24hr")
    FUNC(set_time, 1)
    EXPECT(set_time, hour, 23)
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

#undef TEST_CASE
#undef TEST_DEF
