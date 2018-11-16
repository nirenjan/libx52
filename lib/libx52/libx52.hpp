/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#ifndef LIBX52CLASS_H
#define LIBX52CLASS_H

#include "libx52.h"

/**
 * @brief libx52 class
 */
class libx52 {
    private:
    libx52_device *dev;
    int errcode;

    public:
    /** Constructor for libx52 class */
    libx52();

    /** Destructor for libx52 class */
    ~libx52();

    /**
     * @brief Set the text on an MFD line
     *
     * The Multifunction display (MFD) supports 3 lines of up to 16 characters.
     * This function will set the text into the internal data structures.
     *
     * \p text must be a pointer to an array of bytes, each byte represents a
     * code point on the MFD's internal character map. \p length is the length
     * of this array.
     *
     * \p line must be \b 0, \b 1, or \b 2 and refers to the first, second or
     * third line of the MFD respectively.
     *
     * @par Limitations
     * This function can only store a maximum of 16 characters per line. Any
     * additional characters are silently discarded.
     *
     * @param[in]   line    Line to be updated (0, 1 or 2)
     * @param[in]   text    Pointer to the text string. The text must be mapped
     *                      to the code page of the X52 display.
     * @param[in]   length  Length of the text to display
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if either \p x52
     * is invalid, or \p line is outside the accepted range.
     */
    int set_text(uint8_t line, const char *text, uint8_t length);

    /**
     * @brief Set the LED state
     *
     * The X52 pro has several LEDs that can be individually controlled. This
     * function allows you to set the state of each LED in the internal data
     * structures.
     *
     * Note that the LEDs referred to by \ref LIBX52_LED_FIRE and \ref
     * LIBX52_LED_THROTTLE support only the states \ref LIBX52_LED_STATE_OFF
     * and \ref LIBX52_LED_STATE_ON. The remaining LEDs support all the states
     * with the exception of \ref LIBX52_LED_STATE_ON.
     *
     * @param[in]   led     LED identifier (refer \ref libx52_led_id)
     * @param[in]   state   State of the LED (refer \ref libx52_led_state)
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if the \p x52
     * parameter is not valid, and \ref LIBX52_ERROR_NOT_SUPPORTED if the \p
     * led and \p state combination is not a supported one.  The API also
     * returns \ref LIBX52_ERROR_NOT_SUPPORTED if the probed joystick is not an
     * X52 Pro, but the non-Pro X52 variant.
     */
    int set_led_state(libx52_led_id led, libx52_led_state state);

    /**
     * @brief Set the clock
     *
     * This function sets the primary clock's date and time with the specified
     * Unix time value.  \p time can be obtained from \c time(2). \p local
     * controls whether the primary clock displays local time or GMT.
     *
     * If this function is called again within the same minute as calculated by
     * \c localtime(3) or \c gmtime(3), it will return \ref
     * LIBX52_ERROR_TRY_AGAIN, as it does not require any updates to be written
     * to the joystick. However, if the call changes the timezone from local
     * time to GMT or vice-versa, then the function will return 0, since it
     * requires a write to the device to update the clock with the new
     * timezone.
     *
     * The secondary and tertiary clocks are driven off the primary clock and
     * set using \ref libx52_set_clock_timezone.
     *
     * @param[in]   time    Time value from \c time(3)
     * @param[in]   local   0 for GM time, non-zero for localtime
     *
     * @returns 0 on success, \ref LIBX52_ERROR_TRY_AGAIN if no change from
     * previous time, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not valid.
     */
    int set_clock(time_t time, int local);

    /**
     * @brief Set the timezone for the secondary and tertiary clocks.
     *
     * The X52 Pro has a total of 3 clock displays. The secondary and tertiary
     * clocks are controlled as an offset from the primary clock in minutes.
     * However, for convenience, the X52 library calculates this offset
     * internally and only requires you to set the timezone as the number of
     * minutes offset from GMT. \p offset is limited to +/- 1440 minutes, and
     * any offset outside this range will result in a return value of \ref
     * LIBX52_ERROR_OUT_OF_RANGE
     *
     * @param[in]   clock   \ref libx52_clock_id, cannot be \ref
     *                      LIBX52_CLOCK_1
     * @param[in]   offset  Offset in minutes from GMT (east is positive, west
     *                      is negative)
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is
     * invalid, \ref LIBX52_ERROR_NOT_SUPPORTED if \p clock is \ref
     * LIBX52_CLOCK_1, \ref LIBX52_ERROR_OUT_OF_RANGE if \p offset is more than
     * +/- 24 hours.
     */
    int set_clock_timezone(libx52_clock_id clock, int offset);

    /**
     * @brief Set whether the clock is displayed in 12 hour or 24 hour format.
     *
     * The clocks on the X52 Pro MFD are all displayed as HH:MM, but can be set
     * to display it in 12-hour (AM/PM) or 24-hour format. The default format
     * if not specified is 12-hour.
     *
     * @par Limitations
     * The hardware has a limitation that it cannot display 12:00 am in 12 hour
     * mode - instead it will display as 00:00 am
     *
     * @param[in]   clock   \ref libx52_clock_id
     * @param[in]   format  \ref libx52_clock_format
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid, or if either of \p clock or \p format are outside their
     * respective ranges.
     */
    int set_clock_format(libx52_clock_id clock, libx52_clock_format format);

    /**
     * @brief Set the hour and minute on clock 1
     *
     * This is a raw API which can be used for manual control if the user
     * so desires, however, it will not update the timezone or the date
     * values.
     *
     * @param[in]   hour    Hour to display
     * @param[in]   minute  Minute to display
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid
     */
    int set_time(uint8_t hour, uint8_t minute);

    /**
     * @brief Set the date
     *
     * This is a raw API which can be used for manual control if the user
     * so desires, however, it will not update the timezone or the time values.
     *
     * @param[in]   dd      Day to display
     * @param[in]   mm      Month to display
     * @param[in]   yy      Year to display
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid
     */
    int set_date(uint8_t dd, uint8_t mm, uint8_t yy);

    /**
     * @brief Set the date format for the MFD date display
     *
     * If not set, the date format defaults to DD-MM-YY
     *
     * @param[in]   format  \ref libx52_date_format
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid
     */
    int set_date_format(libx52_date_format format);

    /**
     * @brief Set the MFD or LED brightness
     *
     * The brightness of the MFD display and the button LEDs can be controlled
     * individually. \p brightness should be a value between 0 and 128. While
     * the library does not fail on values higher than 128, the effect may not
     * be what is intended.
     *
     * @param[in]   mfd     0 for LED brightness, 1 for MFD brightness
     * @param[in]   brightness  Brightness level to set
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid.
     */
    int set_brightness(uint8_t mfd, uint16_t brightness);

    /**
     * @brief Set the state of the shift indicator
     *
     * The X52 Pro MFD has a single shift indicator that indicates when the
     * "shift" button is pressed.
     *
     * @param[in]   state   0 for off, 1 for on
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid
     */
    int set_shift(uint8_t state);

    /**
     * @brief Set the blinking state
     *
     * The X52 Pro has a "blink" state where it blinks the clutch and hat LEDs.
     *
     * @param[in]   state   0 for off, 1 for on
     *
     * @returns 0 on success, \ref LIBX52_ERROR_INVALID_PARAM if \p x52 is not
     * valid
     */
    int set_blink(uint8_t state);

    /**
     * @brief Update the X52
     *
     * All the libx52_set functions only set the internal data structures, but
     * do not actually write anything to the joystick. This function writes the
     * saved data to the joystick and updates the internal data structures as
     * necessary.
     *
     * @returns 0 on success, \ref LIBX52_ERROR_USB_FAILURE on failure
     */
    int update(); 

    /**
     * @brief Write a raw vendor control packet
     *
     * This function sends the control packet immediately to the hardware,
     * without having to wait for a call to \ref libx52_update.
     *
     * This can be used to debug issues seen on the hardware, however, it is \b
     * NOT recommended for use by end users, as it can potentially damage the
     * hardware.
     *
     * @param[in]   index   wIndex in the USB packet
     * @param[in]   value   wValue in the USB packet
     *
     * @returns 0 on success, \ref LIBX52_ERROR_USB_FAILURE on failure
     */
    int vendor_command(uint16_t index, uint16_t value);

    /**
     * @brief Return a string representation of the error code
     *
     * @returns Pointer to a NULL terminated string describing the error.
     * Returned pointer must not be freed.
     */
    char * strerror();
};

#endif /* !defined LIBX52CLASS_H */
