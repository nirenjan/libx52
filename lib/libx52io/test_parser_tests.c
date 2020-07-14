/*
 * Saitek X52 IO driver - Parser test suite - individual parser tests
 *
 * This file is included in test_parser.c to both define the test cases,
 * and to list the tests themselves.
 */

#ifndef TEST_LIST
# define TEST_CASE(tc, prodid) static void tc (void **state)
# define TEST_DEF 1
# define TEST_PARSE(...) \
    libx52io_context *ctx = *state; \
    libx52io_report report; \
    int rc; \
    unsigned char data[] = { __VA_ARGS__ }; \
    memset(&report, 0, sizeof(report)); \
    rc = _x52io_parse_report(ctx, &report, data, sizeof(data)); \
    assert_int_equal(rc, LIBX52IO_SUCCESS);
#else
# define TEST_CASE(tc, prodid) cmocka_unit_test_setup_teardown(tc, TEST_SETUP(prodid), test_teardown),
# define TEST_DEF 0
#endif

TEST_CASE(x52_x_axis_512, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 512);
}
#endif

TEST_CASE(x52_x_axis_1024, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 1024);
}
#endif

TEST_CASE(x52_x_axis_2047, _1)
#if TEST_DEF
{
    TEST_PARSE(0xff, 0x07, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_X], 2047);
}
#endif

TEST_CASE(x52_y_axis_512, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 0, 0x10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 512);
}
#endif

TEST_CASE(x52_y_axis_1024, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 0, 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 1024);
}
#endif

TEST_CASE(x52_y_axis_2047, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 0xf8, 0x3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_Y], 2047);
}
#endif

TEST_CASE(x52_rz_axis_256, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 0, 0, 0x40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 256);
}
#endif

TEST_CASE(x52_rz_axis_512, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 0, 0, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 512);
}
#endif

TEST_CASE(x52_rz_axis_1023, _1)
#if TEST_DEF
{
    TEST_PARSE(0, 0, 0xc0, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_int_equal(report.axis[LIBX52IO_AXIS_RZ], 1023);
}
#endif

#undef TEST_CASE
#undef TEST_DEF
#undef TEST_PARSE
