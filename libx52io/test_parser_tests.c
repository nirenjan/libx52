/*
 * Saitek X52 IO driver - Parser test suite - individual parser tests
 *
 * This file is included in test_parser.c to both define the test cases,
 * and to list the tests themselves.
 */

#ifndef TEST_LIST
# define TEST_CASE(tc, prodid) static void tc (void **state)
# define TEST_DEF(x) x

/* Setup the test case with default values in the report data */
# define TEST_INIT(size) \
    libx52io_context *ctx = *state; \
    libx52io_report report; \
    int rc; \
    unsigned char data[size] = { 0 }; \
    memset(&report, 0, sizeof(report));

# define TEST_INIT_X52 TEST_INIT(14)
# define TEST_INIT_PRO TEST_INIT(15)

/* Set the axis bits. The axis values are reported starting from offset 0 */
# define TEST_AXIS(axis) do { \
    data[0] = (axis ## L >>  0) & 0xff; \
    data[1] = (axis ## L >>  8) & 0xff; \
    data[2] = (axis ## L >> 16) & 0xff; \
    data[3] = (axis ## L >> 24) & 0xff; \
    data[4] = (axis ## L >> 32) & 0xff; \
    data[5] = (axis ## L >> 40) & 0xff; \
    data[6] = (axis ## L >> 48) & 0xff; \
    data[7] = (axis ## L >> 56) & 0xff; \
} while(0)

/* Set the button bit. The buttons are the report data starting from offset 8 */
# define TEST_BUTTON(btn) data[8 + (btn >> 3)] |= (1 << (btn & 7))

/*
 * Set the hat value. The hat is in the upper 4 bits of the penultimate byte
 * of the report.
 */
# define TEST_HAT(hat) data[sizeof(data) - 2] |= (hat << 4)

/* Set the thumbstick X and Y values. This is in the last byte of the report. */
# define TEST_THUMB(x, y) data[sizeof(data) - 1] = (y << 4 | x)

/* Parse the report */
# define TEST_PARSE() do { \
    rc = _x52io_parse_report(ctx, &report, data, sizeof(data)); \
    assert_int_equal(rc, LIBX52IO_SUCCESS); \
} while(0)

# define TEST_PARSE_REPORT(...) \
    libx52io_context *ctx = *state; \
    libx52io_report report; \
    int rc; \
    unsigned char data[] = { __VA_ARGS__ }; \
    memset(&report, 0, sizeof(report)); \
    rc = _x52io_parse_report(ctx, &report, data, sizeof(data)); \
    assert_int_equal(rc, LIBX52IO_SUCCESS);

#else
# define TEST_CASE(tc, prodid) cmocka_unit_test_setup_teardown(tc, TEST_SETUP(prodid), test_teardown),
# define TEST_DEF(x)
#endif

/* X52 axis test cases {{{ */
TEST_CASE(x52_x_axis_512, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000000000000200);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 512);
})

TEST_CASE(x52_x_axis_1024, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000000000000400);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 1024);
})

TEST_CASE(x52_x_axis_2047, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x00000000000007ff);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 2047);
})

TEST_CASE(x52_y_axis_512, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000000000100000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 512);
})

TEST_CASE(x52_y_axis_1024, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000000000200000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 1024);
})

TEST_CASE(x52_y_axis_2047, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x00000000003ff800);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 2047);
})

TEST_CASE(x52_rz_axis_256, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000000040000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 256);
})

TEST_CASE(x52_rz_axis_512, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000000080000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 512);
})

TEST_CASE(x52_rz_axis_1023, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x00000000ffc00000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 1023);
})

TEST_CASE(x52_z_axis_128, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000008000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Z], 128);
})

TEST_CASE(x52_z_axis_255, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x000000ff00000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Z], 255);
})

TEST_CASE(x52_rx_axis_255, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x0000ff0000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RX], 255);
})

TEST_CASE(x52_ry_axis_255, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0x00ff000000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RY], 255);
})

TEST_CASE(x52_slider_axis_255, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_AXIS(0xff00000000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_SLIDER], 255);
})
/* }}} */

/* X52Pro axis test cases {{{ */
TEST_CASE(pro_x_axis_512, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x000000000000200);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 512);
})

TEST_CASE(pro_x_axis_1023, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x0000000000003ff);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 1023);
})

TEST_CASE(pro_y_axis_512, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x000000000080000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 512);
})

TEST_CASE(pro_y_axis_1023, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x0000000000ffc00);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 1023);
})

TEST_CASE(pro_rz_axis_256, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x0000000040000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 256);
})

TEST_CASE(pro_rz_axis_512, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x0000000080000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 512);
})

TEST_CASE(pro_rz_axis_1023, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x00000000ffc00000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 1023);
})

TEST_CASE(pro_z_axis_128, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x0000008000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Z], 128);
})

TEST_CASE(pro_z_axis_255, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x000000ff00000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_Z], 255);
})

TEST_CASE(pro_rx_axis_255, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x0000ff0000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RX], 255);
})

TEST_CASE(pro_ry_axis_255, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0x00ff000000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_RY], 255);
})

TEST_CASE(pro_slider_axis_255, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_AXIS(0xff00000000000000);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_SLIDER], 255);
})
/* }}} */

/* X52 button test cases {{{ */
TEST_CASE(x52_button_trigger, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(0);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_TRIGGER]);
})

TEST_CASE(x52_button_fire, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(1);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_FIRE]);
})

TEST_CASE(x52_button_a, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(2);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_A]);
})

TEST_CASE(x52_button_b, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(3);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_B]);
})

TEST_CASE(x52_button_c, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(4);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_C]);
})

TEST_CASE(x52_button_pinky, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(5);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_PINKY]);
})

TEST_CASE(x52_button_d, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(6);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_D]);
})

TEST_CASE(x52_button_e, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(7);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_E]);
})

TEST_CASE(x52_button_t1_up, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(8);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T1_UP]);
})

TEST_CASE(x52_button_t1_dn, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(9);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T1_DN]);
})

TEST_CASE(x52_button_t2_up, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(10);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T2_UP]);
})

TEST_CASE(x52_button_t2_dn, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(11);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T2_DN]);
})

TEST_CASE(x52_button_t3_up, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(12);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T3_UP]);
})

TEST_CASE(x52_button_t3_dn, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(13);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T3_DN]);
})

TEST_CASE(x52_button_trigger_2, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(14);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_TRIGGER_2]);
})

TEST_CASE(x52_button_pov_1_n, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(15);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_N]);
})

TEST_CASE(x52_button_pov_1_e, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(16);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_E]);
})

TEST_CASE(x52_button_pov_1_s, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(17);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_S]);
})

TEST_CASE(x52_button_pov_1_w, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(18);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_W]);
})

TEST_CASE(x52_button_pov_2_n, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(19);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_N]);
})

TEST_CASE(x52_button_pov_2_e, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(20);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_E]);
})

TEST_CASE(x52_button_pov_2_s, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(21);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_S]);
})

TEST_CASE(x52_button_pov_2_w, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(22);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_W]);
})

TEST_CASE(x52_button_mode_1, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(23);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MODE_1]);
    assert_int_equal(report.mode, 1);
})

TEST_CASE(x52_button_mode_2, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(24);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MODE_2]);
    assert_int_equal(report.mode, 2);
})

TEST_CASE(x52_button_mode_3, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(25);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MODE_3]);
    assert_int_equal(report.mode, 3);
})

TEST_CASE(x52_button_function, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(26);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_FUNCTION]);
})

TEST_CASE(x52_button_start_stop, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(27);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_START_STOP]);
})

TEST_CASE(x52_button_reset, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(28);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_RESET]);
})

TEST_CASE(x52_button_clutch, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(29);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_CLUTCH]);
})

TEST_CASE(x52_button_mouse_primary, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(30);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_PRIMARY]);
})

TEST_CASE(x52_button_mouse_secondary, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(31);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_SECONDARY]);
})

TEST_CASE(x52_button_mouse_scroll_dn, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(32);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_SCROLL_DN]);
})

TEST_CASE(x52_button_mouse_scroll_up, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_BUTTON(33);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_SCROLL_UP]);
})
/* }}} */

/* X52Pro button test cases {{{ */
TEST_CASE(pro_button_trigger, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(0);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_TRIGGER]);
})

TEST_CASE(pro_button_fire, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(1);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_FIRE]);
})

TEST_CASE(pro_button_a, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(2);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_A]);
})

TEST_CASE(pro_button_b, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(3);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_B]);
})

TEST_CASE(pro_button_c, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(4);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_C]);
})

TEST_CASE(pro_button_pinky, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(5);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_PINKY]);
})

TEST_CASE(pro_button_d, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(6);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_D]);
})

TEST_CASE(pro_button_e, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(7);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_E]);
})

TEST_CASE(pro_button_t1_up, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(8);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T1_UP]);
})

TEST_CASE(pro_button_t1_dn, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(9);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T1_DN]);
})

TEST_CASE(pro_button_t2_up, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(10);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T2_UP]);
})

TEST_CASE(pro_button_t2_dn, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(11);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T2_DN]);
})

TEST_CASE(pro_button_t3_up, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(12);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T3_UP]);
})

TEST_CASE(pro_button_t3_dn, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(13);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_T3_DN]);
})

TEST_CASE(pro_button_trigger_2, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(14);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_TRIGGER_2]);
})

TEST_CASE(pro_button_mouse_primary, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(15);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_PRIMARY]);
})

TEST_CASE(pro_button_mouse_scroll_dn, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(16);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_SCROLL_DN]);
})

TEST_CASE(pro_button_mouse_scroll_up, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(17);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_SCROLL_UP]);
})

TEST_CASE(pro_button_mouse_secondary, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(18);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MOUSE_SECONDARY]);
})

TEST_CASE(pro_button_pov_1_n, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(19);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_N]);
})

TEST_CASE(pro_button_pov_1_e, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(20);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_E]);
})

TEST_CASE(pro_button_pov_1_s, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(21);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_S]);
})

TEST_CASE(pro_button_pov_1_w, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(22);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_1_W]);
})

TEST_CASE(pro_button_pov_2_n, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(23);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_N]);
})

TEST_CASE(pro_button_pov_2_e, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(24);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_E]);
})

TEST_CASE(pro_button_pov_2_s, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(25);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_S]);
})

TEST_CASE(pro_button_pov_2_w, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(26);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_POV_2_W]);
})

TEST_CASE(pro_button_mode_1, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(27);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MODE_1]);
    assert_int_equal(report.mode, 1);
})

TEST_CASE(pro_button_mode_2, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(28);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MODE_2]);
    assert_int_equal(report.mode, 2);
})

TEST_CASE(pro_button_mode_3, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(29);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_MODE_3]);
    assert_int_equal(report.mode, 3);
})

TEST_CASE(pro_button_clutch, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(30);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_CLUTCH]);
})

TEST_CASE(pro_button_function, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(31);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_FUNCTION]);
})

TEST_CASE(pro_button_start_stop, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(32);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_START_STOP]);
})

TEST_CASE(pro_button_reset, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(33);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_RESET]);
})

TEST_CASE(pro_button_pg_up, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(34);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_PG_UP]);
})

TEST_CASE(pro_button_pg_dn, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(35);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_PG_DN]);
})

TEST_CASE(pro_button_up, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(36);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_UP]);
})

TEST_CASE(pro_button_dn, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(37);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_DN]);
})

TEST_CASE(pro_button_select, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_BUTTON(38);
    TEST_PARSE();

    assert_true(report.button[LIBX52IO_BTN_SELECT]);
})
/* }}} */

/* X52 hat test cases {{{ */
TEST_CASE(x52_hat_0, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(0);
    TEST_PARSE();

    assert_int_equal(report.hat, 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 0);
})

TEST_CASE(x52_hat_1, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(1);
    TEST_PARSE();

    assert_int_equal(report.hat, 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], -1);
})

TEST_CASE(x52_hat_2, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(2);
    TEST_PARSE();

    assert_int_equal(report.hat, 2);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], -1);
})

TEST_CASE(x52_hat_3, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(3);
    TEST_PARSE();

    assert_int_equal(report.hat, 3);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 0);
})

TEST_CASE(x52_hat_4, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(4);
    TEST_PARSE();

    assert_int_equal(report.hat, 4);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 1);
})

TEST_CASE(x52_hat_5, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(5);
    TEST_PARSE();

    assert_int_equal(report.hat, 5);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 1);
})

TEST_CASE(x52_hat_6, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(6);
    TEST_PARSE();

    assert_int_equal(report.hat, 6);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], -1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 1);
})

TEST_CASE(x52_hat_7, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(7);
    TEST_PARSE();

    assert_int_equal(report.hat, 7);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], -1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 0);
})

TEST_CASE(x52_hat_8, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_HAT(8);
    TEST_PARSE();

    assert_int_equal(report.hat, 8);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], -1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], -1);
})
/* }}}*/

/* X52Pro hat test cases {{{ */
TEST_CASE(pro_hat_0, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(0);
    TEST_PARSE();

    assert_int_equal(report.hat, 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 0);
})

TEST_CASE(pro_hat_1, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(1);
    TEST_PARSE();

    assert_int_equal(report.hat, 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], -1);
})

TEST_CASE(pro_hat_2, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(2);
    TEST_PARSE();

    assert_int_equal(report.hat, 2);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], -1);
})

TEST_CASE(pro_hat_3, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(3);
    TEST_PARSE();

    assert_int_equal(report.hat, 3);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 0);
})

TEST_CASE(pro_hat_4, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(4);
    TEST_PARSE();

    assert_int_equal(report.hat, 4);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 1);
})

TEST_CASE(pro_hat_5, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(5);
    TEST_PARSE();

    assert_int_equal(report.hat, 5);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 1);
})

TEST_CASE(pro_hat_6, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(6);
    TEST_PARSE();

    assert_int_equal(report.hat, 6);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], -1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 1);
})

TEST_CASE(pro_hat_7, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(7);
    TEST_PARSE();

    assert_int_equal(report.hat, 7);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], -1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], 0);
})

TEST_CASE(pro_hat_8, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_HAT(8);
    TEST_PARSE();

    assert_int_equal(report.hat, 8);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATX], -1);
    assert_int_equal(report.axis[LIBX52IO_AXIS_HATY], -1);
})
/* }}}*/

/* X52 thumbstick test cases {{{ */
TEST_CASE(x52_thumb_0_0, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_THUMB(0, 0);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0);
})

TEST_CASE(x52_thumb_0_f, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_THUMB(0, 0xf);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0xf);
})

TEST_CASE(x52_thumb_f_0, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_THUMB(0xf, 0);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0xf);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0);
})

TEST_CASE(x52_thumb_f_f, _1)
TEST_DEF({
    TEST_INIT_X52;
    TEST_THUMB(0xf, 0xf);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0xf);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0xf);
})
/* }}} */

/* X52Pro thumbstick test cases {{{ */
TEST_CASE(pro_thumb_0_0, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_THUMB(0, 0);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0);
})

TEST_CASE(pro_thumb_0_f, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_THUMB(0, 0xf);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0xf);
})

TEST_CASE(pro_thumb_f_0, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_THUMB(0xf, 0);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0xf);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0);
})

TEST_CASE(pro_thumb_f_f, PRO)
TEST_DEF({
    TEST_INIT_PRO;
    TEST_THUMB(0xf, 0xf);
    TEST_PARSE();

    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBX], 0xf);
    assert_int_equal(report.axis[LIBX52IO_AXIS_THUMBY], 0xf);
})
/* }}} */

#undef TEST_CASE
#undef TEST_DEF
#undef TEST_PARSE
