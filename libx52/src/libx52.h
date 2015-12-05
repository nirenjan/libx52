/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#ifndef LIBX52_H
#define LIBX52_H

#include <time.h>
#include <stdint.h>

/**
 * @brief Opaque structure used by libx52
 */
struct libx52_device;

/**
 * @brief Opaque structure used by libx52
 */
typedef struct libx52_device libx52_device;

/**
 * @brief List of supported clocks on the MFD
 */
typedef enum {
    /** Primary clock on the MFD, indicated by the number 1 */
    LIBX52_CLOCK_1,

    /** Secondary clock on the MFD, indicated by the number 2 */
    LIBX52_CLOCK_2,

    /** Tertiary clock on the MFD, indicated by the number 3 */
    LIBX52_CLOCK_3,
} libx52_clock_id;

/**
 * @brief Supported clock formats
 */
typedef enum {
    /** 12-hour display on the MFD clock */
    LIBX52_CLOCK_FORMAT_12HR,

    /** 24-hour display on the MFD clock */
    LIBX52_CLOCK_FORMAT_24HR,
} libx52_clock_format;

/**
 * @brief Supported date formats
 */
typedef enum {
    /** Date format DD-MM-YY */
    LIBX52_DATE_FORMAT_DDMMYY,

    /** Date format MM-DD-YY */
    LIBX52_DATE_FORMAT_MMDDYY,

    /** Date format YY-MM-DD */
    LIBX52_DATE_FORMAT_YYMMDD,
} libx52_date_format;

/**
 * @brief Supported LED identifiers
 */
typedef enum {
    /** Fire indicator LED */
    LIBX52_LED_FIRE = 0x01,

    /** LED on button A */
    LIBX52_LED_A = 0x02,

    /** LED on button B */
    LIBX52_LED_B = 0x04,

    /** LED on button D */
    LIBX52_LED_D = 0x06,

    /** LED on button E */
    LIBX52_LED_E = 0x08,

    /** LED on toggle T1 */
    LIBX52_LED_T1 = 0x0a,

    /** LED on toggle T2 */
    LIBX52_LED_T2 = 0x0c,

    /** LED on toggle T3 */
    LIBX52_LED_T3 = 0x0e,

    /** LED on POV Hat */
    LIBX52_LED_POV = 0x10,

    /** LED on clutch button */
    LIBX52_LED_CLUTCH = 0x12,

    /** Throttle LED */
    LIBX52_LED_THROTTLE = 0x14,
} libx52_led_id;

/**
 * @brief Supported LED states
 *
 * Not all LEDs support all states
 */
typedef enum {
    /** LED is off */
    LIBX52_LED_STATE_OFF,

    /**
     * LED is on. This is not supported by those LEDs which can display
     * multiple colors
     */
    LIBX52_LED_STATE_ON,

    /** LED displays red color. This is not supported by single state LEDs */
    LIBX52_LED_STATE_RED,

    /** LED displays amber color. This is not supported by single state LEDs */
    LIBX52_LED_STATE_AMBER,

    /** LED displays green color. This is not supported by single state LEDs */
    LIBX52_LED_STATE_GREEN,
} libx52_led_state;

/**
 * @brief Initialize the X52 library
 *
 * Sets up any internal data structures to access the joystick, all
 * calls to libx52 use the returned pointer to control the device.
 *
 * @returns a pointer to the detected \ref libx52_device
 */
libx52_device * libx52_init(void);

/**
 * @brief Exit the library and free up any resources used
 *
 * @param[in]   dev     Pointer to the device
 * @returns None
 */
void libx52_exit(libx52_device *dev);

/**
 * @brief Set the text on an MFD line
 *
 * The Multifunction display (MFD) supports 3 lines of up to 16 characters.
 * This function will set the text into the internal data structures.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   line    Line to be updated (0, 1 or 2)
 * @param[in]   text    Pointer to the text string. The text must be mapped to
 *                      the code page of the X52 display.
 * @param[in]   length  Length of the text to display
 *
 * @returns 0 on success, -errno on failure.
 */
int libx52_set_text(libx52_device *x52, uint8_t line, const char *text, uint8_t length);

/**
 * @brief Set the LED state
 *
 * The X52 pro has several LEDs that can be individually controlled. This
 * function will allow you to control them.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   led     LED identifier (refer \ref libx52_led_id)
 * @param[in]   state   State of the LED (refer \ref libx52_led_state)
 *
 * @returns 0 on success, -errno on failure.
 */
int libx52_set_led_state(libx52_device *x52,
                         libx52_led_id led,
                         libx52_led_state state);

/**
 * @brief Set the clock
 *
 * This function sets the primary clock's date and time with the specified Unix
 * time value. An additional argument specifies whether this uses local time or
 * GM time.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   time    Time value from \ref time(3)
 * @param[in]   local   0 for GM time, non-zero for localtime
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_clock(libx52_device *x52, time_t time, int local);

/**
 * @brief Set the timezone for the secondary and tertiary clocks.
 *
 * The X52 Pro has a total of 3 clock displays. The secondary and tertiary
 * clocks are controlled as an offset from the primary clock in minutes.
 * However, for convenience, the X52 library calculates this offset internally
 * and only requires you to set the timezone as the number of minutes offset
 * from GMT. Offset is limited to +/- 1440 minutes, and any offset outside
 * this range will result in a return value of -EDOM
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   clock   \ref libx52_clock_id, cannot be \ref
 *                      LIBX52_CLOCK_1
 * @param[in]   offset  Offset in minutes from GMT (east is positive, west
 *                      is negative)
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_clock_timezone(libx52_device *x52,
                              libx52_clock_id clock,
                              int offset);

/**
 * @brief Set whether the clock is displayed in 12 hour or 24 hour format.
 *
 * The clocks on the X52 Pro MFD are all displayed as HH:MM, but can be set to
 * display it in 12-hour (AM/PM) or 24-hour format. The default format if not
 * specified is 12-hour.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   clock   \ref libx52_clock_id
 * @param[in]   format  \ref libx52_clock_format
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_clock_format(libx52_device *x52,
                            libx52_clock_id clock,
                            libx52_clock_format format);

/**
 * @brief Set the date format for the MFD date display
 *
 * If not set, the date format defaults to DD-MM-YY
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   format  \ref libx52_date_format
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_date_format(libx52_device *x52, libx52_date_format format);

/**
 * @brief Set the MFD or LED brightness
 *
 * The brightness of the MFD display and the button LEDs can be controlled
 * individually.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   mfd     0 for LED brightness, 1 for MFD brightness
 * @param[in]   brightness  Brightness level to set
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_brightness(libx52_device *x52, uint8_t mfd, uint16_t brightness);

/**
 * @brief Set the state of the shift indicator
 *
 * The X52 Pro MFD has a single shift indicator that indicates when the
 * "shift" button is pressed.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   state   0 for off, 1 for on
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_shift(libx52_device *x52, uint8_t state);

/**
 * @brief Set the blinking state
 *
 * The X52 Pro has a "blink" state where it blinks the clutch and hat LEDs.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   state   0 for off, 1 for on
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_set_blink(libx52_device *x52, uint8_t state);

/**
 * @brief Update the X52
 *
 * All the libx52_set functions only set the internal data structures, but do
 * not actually write anything to the joystick. This function writes the saved
 * data to the joystick and resets the internal data structures.
 *
 * @param[in]   x52     Pointer to the device
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_update(libx52_device *x52);

/**
 * @brief Write a raw vendor control packet
 *
 * This can be used to debug issues seen on the hardware, however, it is not
 * recommended for use by end users, as it can potentially damage the hardware.
 *
 * @param[in]   x52     Pointer to the device
 * @param[in]   index   wIndex in the USB packet
 * @param[in]   value   wValue in the USB packet
 *
 * @returns 0 on success, -errno on failure
 */
int libx52_vendor_command(libx52_device *x52, uint16_t index, uint16_t value);

#endif /* !defined LIBX52_H */
