/*
 * Saitek X52 Pro HOTAS driver
 *
 * Copyright (C) 2012 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#ifdef CONFIG_USB_DEBUG
    #define DEBUG   1
#endif

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>

#include "x52joy_commands.h"
#include "x52joy_common.h"

/**********************************************************************
 * MFD Line manipulation functions
 *********************************************************************/
static const u8 line_cmd[X52_MFD_LINES] = {
    X52_MFD_LINE1,
    X52_MFD_LINE2,
    X52_MFD_LINE3,
};

int set_text(struct x52_joy *joy, u8 line_no)
{
    u16 i;
    u16 ch;
    u16 length;
    char *text_ptr;
    int retval;

    if (!joy || line_no >= X52_MFD_LINES) {
        /* Just some sanity checking */
        return -EINVAL;
    }

    /* Clear the line first */
    retval = usb_control_msg(joy->udev,
        usb_sndctrlpipe(joy->udev, 0),
        X52_VENDOR_REQUEST,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
        0x00,
        line_cmd[line_no] | X52_MFD_CLEAR_LINE,
        NULL, 0, 1000);
    if (retval) {
        return retval;
    }

    length = joy->line[line_no].length;

    for (i = 0; i < X52_MFD_LINE_SIZE && i < length; i+= 2) {
        text_ptr = &joy->line[line_no].text[i];
        if (length - i > 1) {
            ch = *(u16 *)text_ptr;
        } else {
            ch = (*text_ptr) + (0x20 << 8);
        }

        retval = usb_control_msg(joy->udev,
            usb_sndctrlpipe(joy->udev, 0),
            X52_VENDOR_REQUEST,
            USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
            ch,
            line_cmd[line_no] | X52_MFD_WRITE_LINE,
            NULL, 0, 1000);
        if (retval) {
            return retval;
        }
    }

    return 0;
}

/**********************************************************************
 * Brightness manipulation functions
 *********************************************************************/
int set_brightness(struct x52_joy *joy, u8 target)
{
    u16 bri;
    u8 ch;
    int retval;

    if (!joy || target > 1) {
        /* Just some sanity checking */
        return -EINVAL;
    }

    if (target == 0) {  // MFD
        ch = X52_MFD_BRIGHTNESS;
        bri = joy->bri_mfd;
    } else {    // LED
        ch = X52_LED_BRIGHTNESS;
        bri = joy->bri_led;
    }

    if (bri > 0x80) {
        return -EOPNOTSUPP;
    }

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                bri, ch,
                NULL, 0, 1000);
    return retval;
}

/**********************************************************************
 * LED manipulation functions
 *********************************************************************/
int set_led(struct x52_joy *joy, u8 target)
{
    int retval;
    u8 status;
    
    if (!joy || target > X52_LED_THROTTLE) {
        /* Just some sanity checking */
        return -EINVAL;
    }

    status = (u8)((joy->led_status >> target) & 1);

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                target << 8 | status, X52_LED,
                NULL, 0, 1000);
    return retval;
}

/**********************************************************************
 * Date manipulation functions
 *********************************************************************/
int set_date(struct x52_joy *joy)
{
    int retval;
    u16 ddmm;
    u16 yy;
    
    if (!joy) {
        /* Just some sanity checking */
        return -EINVAL;
    }

    /*
     * The X52 Pro MFD expects the date to be stored as DD-MM-YY.
     * However, by tweaking the USB control messages, we can display
     * in any format we choose.
     */
    if (joy->date.format >= x52_mfd_format_max) {
        return -EOPNOTSUPP;
    }

    switch (joy->date.format) {
    case x52_mfd_format_yymmdd:
        yy = joy->date.day;
        ddmm = (joy->date.year << 8) | joy->date.month;
        break;

    case x52_mfd_format_mmddyy:
        yy = joy->date.year;
        ddmm = (joy->date.day << 8) | joy->date.month;
        break;

    case x52_mfd_format_ddmmyy:
        yy = joy->date.year;
        ddmm = (joy->date.month << 8) | joy->date.day;
        break;

    default:
        return -EOPNOTSUPP;
    }

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                ddmm, X52_DATE_DDMM,
                NULL, 0, 1000);
    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                yy, X52_DATE_YEAR,
                NULL, 0, 1000);
    return retval;
}

/**********************************************************************
 * Shift manipulation functions
 *********************************************************************/
int set_shift(struct x52_joy *joy)
{
    int retval;
    int val;
    
    if (!joy) {
        /* Just some sanity checking */
        return -EINVAL;
    }

    if (!joy->feat_mfd) {
        return -EOPNOTSUPP;
    }

    if (joy->shift_ind) {
        val = X52_SHIFT_ON;
    } else {
        val = X52_SHIFT_OFF;
    }

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                val, X52_SHIFT_INDICATOR,
                NULL, 0, 1000);
    return retval;
}

/**********************************************************************
 * Blink manipulation functions
 *********************************************************************/
int set_blink(struct x52_joy *joy)
{
    int retval;
    int val;
    
    if (!joy) {
        /* Just some sanity checking */
        return -EINVAL;
    }

    if (!joy->feat_led) {
        return -EOPNOTSUPP;
    }

    if (joy->blink_led) {
        val = X52_BLINK_ON;
    } else {
        val = X52_BLINK_OFF;
    }

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                val, X52_BLINK_INDICATOR,
                NULL, 0, 1000);
    return retval;
}

