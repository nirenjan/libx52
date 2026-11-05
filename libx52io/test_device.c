/*
 * Saitek X52 IO driver - device info test suite
 *
 * Copyright (C) 2012-2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <wchar.h>

#include "common.h"
#include "usb-ids.h"

static void test_device_info(void **state)
{
    struct hid_device_info dev_info = {0};
    libx52io_context ctx = {0};

    (void)state;

    dev_info.vendor_id = VENDOR_SAITEK;
    dev_info.product_id = X52_PROD_X52PRO;
    dev_info.release_number = 0x1337;
    dev_info.manufacturer_string = L"Saitek";
    dev_info.product_string = L""; // Handle an empty string
    dev_info.serial_number = NULL; // Handle a NULL pointer

    _x52io_save_device_info(&ctx, &dev_info);

    assert_int_equal(libx52io_get_vendor_id(&ctx), VENDOR_SAITEK);
    assert_int_equal(libx52io_get_product_id(&ctx), X52_PROD_X52PRO);
    assert_int_equal(libx52io_get_device_version(&ctx), 0x1337);
    assert_string_equal(libx52io_get_manufacturer_string(&ctx), "Saitek");
    assert_string_equal(libx52io_get_product_string(&ctx), "");
    assert_null(libx52io_get_serial_number_string(&ctx));

    _x52io_release_device_info(&ctx);
}

static void test_null_handling(void **state)
{
    (void)state;
    assert_int_equal(libx52io_get_vendor_id(NULL), 0);
    assert_int_equal(libx52io_get_product_id(NULL), 0);
    assert_int_equal(libx52io_get_device_version(NULL), 0);
    assert_null(libx52io_get_manufacturer_string(NULL));
    assert_null(libx52io_get_product_string(NULL));
    assert_null(libx52io_get_serial_number_string(NULL));
}

const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_device_info),
    cmocka_unit_test(test_null_handling),
};

int main(void)
{
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
