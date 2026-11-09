/*
 * Saitek X52 daemon event handler test suite
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdatomic.h>
#include <errno.h>
#include <cmocka.h>

#include <libx52/libx52io.h>

// Mocked dependencies called by our module under test
uint64_t libx52io_compare_reports(const libx52io_report *prev, const libx52io_report *curr) {
    // Grab the mock return value defined by the test driver
    return (uint64_t)mock();
}

// THE TRICK: Directly include the source file to expose its internal static state
#include "event_handler.c"

/* --- 1. MOCK CALLBACK FUNCTIONS --- */
static int g_callback_count_1 = 0;
static void mock_callback_1(uint64_t mask, const libx52io_report *prev, const libx52io_report *curr) {
    g_callback_count_1++;
    assert_int_equal(mask, 0x0F); // Verify the callback receives the correct mask
}

/* --- 2. SETUP / TEARDOWN FIXTURES --- */
static int clear_table_state(void **state) {
    // Reset our static atomic counter and table state between tests
    atomic_store(&num_handlers, 0);
    for (int i = 0; i < MAX_EVENT_HANDLERS; i++) {
        atomic_store(&handlers[i].fn, NULL);
        atomic_store(&handlers[i].mask, 0);
    }
    g_callback_count_1 = 0;
    return 0;
}

/* --- 3. THE UNIT TESTS --- */

// Test basic parameters and expected input failures
static void test_register_null_callback(void **state) {
    errno = 0;
    int ticket = x52d_register_event_handler(0xFF, NULL);
    assert_int_equal(ticket, -1);
    assert_int_equal(errno, EINVAL);
}

// Test successful registration and ticket bounds constraints
static void test_register_until_full(void **state) {
    for (int i = 0; i < MAX_EVENT_HANDLERS; i++) {
        int ticket = x52d_register_event_handler(0x01, mock_callback_1);
        assert_int_equal(ticket, i); // Ticket should match incremental sequence
    }

    // The 9th registration attempt should fail with ENOSPC
    errno = 0;
    int overflow_ticket = x52d_register_event_handler(0x01, mock_callback_1);
    assert_int_equal(overflow_ticket, -1);
    assert_int_equal(errno, ENOSPC);
}

// Test runtime mask updates and out-of-bound protections
static void test_update_mask_out_of_bounds(void **state) {
    errno = 0;
    int status = x52d_update_event_handler_mask(-1, 0xFF);
    assert_int_equal(status, -1);
    assert_int_equal(errno, ERANGE);

    errno = 0;
    status = x52d_update_event_handler_mask(MAX_EVENT_HANDLERS, 0xFF);
    assert_int_equal(status, -1);
    assert_int_equal(errno, ERANGE);
}

// Test mask updates on an unregistered handle
static void test_update_mask_invalid_entry(void **state) {
    errno = 0;
    int status = x52d_update_event_handler_mask(0, 0xFF);
    assert_int_equal(status, -1);
    assert_int_equal(errno, EINVAL);
}

// Test mask updates on a registered handle
static void test_update_mask_valid_entry(void **state) {
    int handle = x52d_register_event_handler(0x0F, mock_callback_1);
    assert_int_equal(handle, 0);

    int status = x52d_update_event_handler_mask(handle, 0xFF);
    assert_int_equal(status, 0);
    assert_int_equal(atomic_load(&handlers[handle].mask), 0xFF);
}

// Test dispatch functionality and mask matching logic
static void test_dispatch_matching_event(void **state) {
    libx52io_report prev, curr;

    // Register handler matching mask bit 0x0F
    int ticket = x52d_register_event_handler(0x0F, mock_callback_1);
    assert_true(ticket >= 0);

    // Setup our mock return for libx52io_compare_reports
    will_return(libx52io_compare_reports, 0x0F); // Report changed bit matches mask

    // Run the dispatch engine
    x52d_dispatch_event(&prev, &curr);

    // Verify callback executed exactly once
    assert_int_equal(g_callback_count_1, 1);
}

// Test dispatch with NULL pointer doesn't crash the program
static void test_dispatch_empty_table(void **state) {
    libx52io_report prev, curr;

    // Simulate a corrupted table with num_handlers set to maximum, but empty table
    atomic_store(&num_handlers, MAX_EVENT_HANDLERS);

    // Setup our mock return for libx52io_compare_reports
    will_return(libx52io_compare_reports, 0x0F);

    // Run the dispatch engine
    x52d_dispatch_event(&prev, &curr);
    //
    // Verify callback never executed
    assert_int_equal(g_callback_count_1, 0);
}

// Test dispatch functionality with empty mask
static void test_dispatch_empty_mask(void **state) {
    libx52io_report prev, curr;

    // Register handler matching empty mask
    int ticket = x52d_register_event_handler(0, mock_callback_1);
    assert_true(ticket >= 0);

    // Setup our mock return for libx52io_compare_reports
    will_return(libx52io_compare_reports, 0x0F); // Report changed bit matches mask

    // Run the dispatch engine
    x52d_dispatch_event(&prev, &curr);

    // Verify callback never executed
    assert_int_equal(g_callback_count_1, 0);
}

/* --- 4. ENGINE RUNNER --- */
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_register_null_callback, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_register_until_full, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_update_mask_out_of_bounds, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_update_mask_invalid_entry, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_update_mask_valid_entry, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_dispatch_matching_event, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_dispatch_empty_table, clear_table_state, NULL),
        cmocka_unit_test_setup_teardown(test_dispatch_empty_mask, clear_table_state, NULL),
    };

    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}

