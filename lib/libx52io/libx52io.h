/*
 * Saitek X52 IO driver
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file libx52io.h
 * @brief Functions, structures and enumerations for the Saitek X52 IO driver
 * library.
 *
 * This file contains the type, enum and function prototypes for the Saitek X52
 * IO driver library. These functions allow an application to connect to a
 * supported X52/X52Pro joystick and read the state of the buttons and axes.
 *
 * @author Nirenjan Krishnan (nirenjan@nirenjan.org)
 */
#ifndef LIBX52IO_H
#define LIBX52IO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup libx52io IO Library APIs
 *
 * These functions allow an application to connect to a supported X52/X52Pro
 * joystick and read the state of the buttons and axes.
 *
 * @{
 */

/**
 * @brief Opaque structure used by libx52io
 */
struct libx52io_context;

/**
 * @brief Device context structure used by libx52io
 *
 * All libx52io API functions require the application to pass in a pointer to
 * a valid device context structure. A pointer can be obtained by calling
 * \ref libx52io_init
 */
typedef struct libx52io_context libx52io_context;

/**
 * @brief libx52 IO error codes
 *
 * Error codes returned by libx52io
 */
typedef enum {
    /** No error, indicates success */
    LIBX52IO_SUCCESS,

    /** Initialization failure */
    LIBX52IO_ERROR_INIT_FAILURE,

    /** No compatible device found */
    LIBX52IO_ERROR_NO_DEVICE,

    /** Invalid arguments for function */
    LIBX52IO_ERROR_INVALID,

    /** Connection error */
    LIBX52IO_ERROR_CONN,

    /** Read error from device */
    LIBX52IO_ERROR_IO,

    /** Timeout during read from device */
    LIBX52IO_ERROR_TIMEOUT,
} libx52io_error_code;

/**
 * @brief X52 Axis definitions
 */
typedef enum {
    /** Stick X axis */
    LIBX52IO_AXIS_X,

    /** Stick Y axis */
    LIBX52IO_AXIS_Y,

    /** Stick twist axis */
    LIBX52IO_AXIS_RZ,

    /** Throttle axis */
    LIBX52IO_AXIS_Z,

    /** Throttle Rotary X */
    LIBX52IO_AXIS_RX,

    /** Throttle Rotary Y */
    LIBX52IO_AXIS_RY,

    /** Throttle Slider */
    LIBX52IO_AXIS_SLIDER,

    /** Thumbstick X */
    LIBX52IO_AXIS_THUMBX,

    /** Thumbstick Y */
    LIBX52IO_AXIS_THUMBY,

    /** Hat X */
    LIBX52IO_AXIS_HATX,

    /** Hat Y */
    LIBX52IO_AXIS_HATY,

    LIBX52IO_AXIS_MAX
} libx52io_axis;

/**
 * @brief X52 Button definitions
 */
typedef enum {
    /** Primary trigger */
    LIBX52IO_BTN_TRIGGER,

    /** Secondary trigger */
    LIBX52IO_BTN_TRIGGER_2,

    /** Fire button */
    LIBX52IO_BTN_FIRE,

    /** Pinky trigger */
    LIBX52IO_BTN_PINKY,

    /** A button, on stick */
    LIBX52IO_BTN_A,

    /** B button, on stick */
    LIBX52IO_BTN_B,

    /** C button, on stick */
    LIBX52IO_BTN_C,

    /** D button, on throttle */
    LIBX52IO_BTN_D,

    /** E button, on throttle */
    LIBX52IO_BTN_E,

    /** Toggle 1 up */
    LIBX52IO_BTN_T1_UP,

    /** Toggle 1 down */
    LIBX52IO_BTN_T1_DN,

    /** Toggle 2 up */
    LIBX52IO_BTN_T2_UP,

    /** Toggle 2 down */
    LIBX52IO_BTN_T2_DN,

    /** Toggle 3 up */
    LIBX52IO_BTN_T3_UP,

    /** Toggle 3 down */
    LIBX52IO_BTN_T3_DN,

    /** POV 1 Up, on stick */
    LIBX52IO_BTN_POV_1_N,

    /** POV 1 Right, on stick */
    LIBX52IO_BTN_POV_1_E,

    /** POV 1 Down, on stick */
    LIBX52IO_BTN_POV_1_S,

    /** POV 1 Left, on stick */
    LIBX52IO_BTN_POV_1_W,

    /** POV 2 Up, on throttle */
    LIBX52IO_BTN_POV_2_N,

    /** POV 2 Right, on throttle */
    LIBX52IO_BTN_POV_2_E,

    /** POV 2 Down, on throttle */
    LIBX52IO_BTN_POV_2_S,

    /** POV 2 Left, on throttle */
    LIBX52IO_BTN_POV_2_W,

    /** Clutch button, on throttle */
    LIBX52IO_BTN_CLUTCH,

    /** Primary mouse button, next to thumbstick */
    LIBX52IO_BTN_MOUSE_PRIMARY,

    /** Secondary mouse button, press scroll wheel on throttle */
    LIBX52IO_BTN_MOUSE_SECONDARY,

    /** Scroll wheel up, on throttle */
    LIBX52IO_BTN_MOUSE_SCROLL_UP,

    /** Scroll wheel down, on throttle */
    LIBX52IO_BTN_MOUSE_SCROLL_DN,

    /** Function button */
    LIBX52IO_BTN_FUNCTION,

    /** Start/Stop button */
    LIBX52IO_BTN_START_STOP,

    /** Reset button */
    LIBX52IO_BTN_RESET,

    /** Page Up button, X52 Pro only */
    LIBX52IO_BTN_PG_UP,

    /** Page Down button, X52 Pro only */
    LIBX52IO_BTN_PG_DN,

    /** Up button, X52 Pro only */
    LIBX52IO_BTN_UP,

    /** Down button, X52 Pro only */
    LIBX52IO_BTN_DN,

    /** Select button, X52 Pro only */
    LIBX52IO_BTN_SELECT,

    /** Mode 1 - reported as a button */
    LIBX52IO_BTN_MODE_1,

    /** Mode 2 - reported as a button */
    LIBX52IO_BTN_MODE_2,

    /** Mode 3 - reported as a button */
    LIBX52IO_BTN_MODE_3,

    LIBX52IO_BUTTON_MAX
} libx52io_button;

/**
 * @brief X52 HID Report
 *
 * This structure holds a parsed HID report
 */
struct libx52io_report {
    /** Axis values */
    int32_t axis[LIBX52IO_AXIS_MAX];

    /** Button values, true is pressed */
    bool button[LIBX52IO_BUTTON_MAX];

    /** Current mode - 1, 2 or 3 */
    uint8_t mode;

    /** Hat position 0-8 */
    uint8_t hat;
};

/**
 * @brief X52 HID Report
 *
 * This structure holds a parsed HID report
 */
typedef struct libx52io_report libx52io_report;

/**
 * @brief Initialize the IO library
 *
 * This function initializes the libx52io library, sets up any internal data
 * structures to access the joystick, and returns a \ref libx52io_context
 * pointer in the output parameter. All calls to libx52io use the returned
 * pointer to control the device.
 *
 * @par Example
 * @code
 * int rc;
 * libx52io_context *ctx;
 * rc = libx52io_init(&ctx);
 * if (rc != LIBX52IO_SUCCESS) {
 *     // Error handling omitted for brevity
 * }
 *
 * // Save ctx for use later
 * @endcode
 *
 * @param[out]  ctx     Pointer to a \ref libx52io_context *. This function will
 * allocate a device context and return the pointer to the context in this variable.
 *
 * @returns \c libx52io_error_code indicating status
 */
int libx52io_init(libx52io_context **ctx);

/**
 * @brief Exit the library and free up any resources used
 *
 * This function releases any resources allocated by \ref libx52io_init and
 * terminates the library. Using the freed device now is invalid and can
 * cause errors.
 *
 * @param[in]   ctx     Pointer to the device context
 * @returns None
 */
void libx52io_exit(libx52io_context *ctx);

/**
 * @brief Open a connection to a supported joystick
 *
 * This function scans for and opens a connection to a supported X52/X52Pro
 * joystick. If no supported joystick is found, it will return \ref
 * LIBX52IO_ERROR_NO_DEVICE.
 *
 * @param[in]   ctx     Pointer to the device context
 *
 * @returns
 * - \ref LIBX52IO_SUCCESS on successful opening
 * - \ref LIBX52IO_ERROR_INVALID if the context pointer is not valid
 * - \ref LIBX52IO_ERROR_NO_DEVICE if no supported joystick is found
 * - \ref LIBX52IO_ERROR_CONN if the connection fails
 */
int libx52io_open(libx52io_context *ctx);

/**
 * @brief Close an existing connection to a supported joystick
 *
 * This function closes any existing connection to a joystick. It is acceptable
 * to call this function if no connection exists.
 *
 * @param[in]   ctx     Pointer to the device context
 *
 * @returns
 * - \ref LIBX52IO_SUCCESS on closing, or if the connection is already closed.
 * - \ref LIBX52IO_ERROR_INVALID if the context pointer is not valid
 */
int libx52io_close(libx52io_context *ctx);

/**
 * @brief Read and parse a HID report
 *
 * This function reads and parses a HID report from a connected joystick. This
 * function will block until some data is available from the joystick, or the
 * timeout is hit, whichever is first.
 *
 * @param[in]   ctx     Pointer to the device context
 * @param[out]  report  Pointer to save the decoded HID report
 * @param[in]   timeout Timeout value in milliseconds
 *
 * @returns
 * - \ref LIBX52IO_SUCCESS on read and parse success
 * - \ref LIBX52IO_ERROR_INVALID if the context or report pointers are not valid
 * - \ref LIBX52IO_ERROR_NO_DEVICE if the device is disconnected
 * - \ref LIBX52IO_ERROR_IO if there was an error reading from the device,
 *   including if the device was disconnected during the read.
 * - \ref LIBX52IO_ERROR_TIMEOUT if no report was read before timeout.
 */
int libx52io_read_timeout(libx52io_context *ctx, libx52io_report *report, int timeout);

/**
 * @brief Read and parse a HID report
 *
 * This behaves the same as \ref libx52io_read_timeout with a timeout of \c -1.
 * This function will block until some data is available from the joystick.
 *
 * @param[in]   ctx     Pointer to the device context
 * @param[out]  report  Pointer to save the decoded HID report
 *
 * @returns
 * - \ref LIBX52IO_SUCCESS on read and parse success
 * - \ref LIBX52IO_ERROR_INVALID if the context or report pointers are not valid
 * - \ref LIBX52IO_ERROR_NO_DEVICE if the device is disconnected
 * - \ref LIBX52IO_ERROR_IO if there was an error reading from the device,
 *   including if the device was disconnected during the read.
 */
int libx52io_read(libx52io_context *ctx, libx52io_report *report);

/**
 * @brief Retrieve the range of an axis
 *
 * This saves the minimum and maximum values of the requested axis in the output
 * parameters. This will only be valid if the device is connected.
 *
 * @param[in]   ctx     Pointer to the device context
 * @param[in]   axis    Axis identifier - see \ref libx52io_axis
 * @param[out]  min     Pointer to save the axis minimum value
 * @param[out]  max     Pointer to save the axis maximum value
 *
 * @returns
 * - \ref LIBX52IO_SUCCESS on read and parse success
 * - \ref LIBX52IO_ERROR_INVALID if the context or output pointers are not
 *   valid, or the requested axis is not a valid axis identifier
 * - \ref LIBX52IO_ERROR_NO_DEVICE if the device is disconnected
 */
int libx52io_get_axis_range(libx52io_context *ctx, libx52io_axis axis, int32_t *min, int32_t *max);

/**
 * @brief Get the string representation of an error code
 *
 * @param[in]   code    Return code from one of the libx52io APIs
 *
 * @returns String representation of the error. This pointer must not be freed.
 */
const char * libx52io_strerror(libx52io_error_code code);

/**
 * @brief Get the string representation of an axis.
 *
 * @param[in]   axis    Axis ID - see \ref libx52io_axis
 *
 * @returns String representation of the axis. This pointer must not be freed.
 * If axis is outside the defined range, then this returns NULL.
 */
const char * libx52io_axis_to_str(libx52io_axis axis);

/**
 * @brief Get the string representation of a button.
 *
 * @param[in]   button  Button ID - see \ref libx52io_button
 *
 * @returns String representation of the button. This pointer must not be freed.
 * If button is outside the defined range, then this returns NULL.
 */
const char * libx52io_button_to_str(libx52io_button button);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // !defined LIBX52IO_H
