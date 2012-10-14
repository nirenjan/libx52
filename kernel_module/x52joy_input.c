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

#include <linux/input.h>
#include <linux/usb/input.h>

#include "x52joy_commands.h"
#include "x52joy_common.h"

#define X52PRO_X_SHIFT  0
#define X52PRO_X_MASK   0x3ff
#define X52PRO_Y_SHIFT  10
#define X52PRO_Y_MASK   0x3ff
#define X52PRO_RZ_SHIFT 20
#define X52PRO_RZ_MASK  0x3ff

static void x52pro_decode_urb(struct x52_joy *joy, unsigned char *data)
{
    struct input_dev *idev = joy->idev;
    u32 stick_axis = le32_to_cpu(&data[0]);

    input_report_key(idev, BTN_TRIGGER_HAPPY1, data[8] & 0x01);
    input_report_key(idev, BTN_TRIGGER_HAPPY2, data[8] & 0x02);
    input_report_key(idev, BTN_TRIGGER_HAPPY3, data[8] & 0x04);
    input_report_key(idev, BTN_TRIGGER_HAPPY4, data[8] & 0x08);
    input_report_key(idev, BTN_TRIGGER_HAPPY5, data[8] & 0x10);
    input_report_key(idev, BTN_TRIGGER_HAPPY6, data[8] & 0x20);
    input_report_key(idev, BTN_TRIGGER_HAPPY7, data[8] & 0x40);
    input_report_key(idev, BTN_TRIGGER_HAPPY8, data[8] & 0x80);

    input_report_key(idev, BTN_TRIGGER_HAPPY9, data[9] & 0x01);
    input_report_key(idev, BTN_TRIGGER_HAPPY10, data[9] & 0x02);
    input_report_key(idev, BTN_TRIGGER_HAPPY11, data[9] & 0x04);
    input_report_key(idev, BTN_TRIGGER_HAPPY12, data[9] & 0x08);
    input_report_key(idev, BTN_TRIGGER_HAPPY13, data[9] & 0x10);
    input_report_key(idev, BTN_TRIGGER_HAPPY14, data[9] & 0x20);
    input_report_key(idev, BTN_TRIGGER_HAPPY15, data[9] & 0x40);
    input_report_key(idev, BTN_TRIGGER_HAPPY16, data[9] & 0x80);

    input_report_key(idev, BTN_TRIGGER_HAPPY17, data[10] & 0x01);
    input_report_key(idev, BTN_TRIGGER_HAPPY18, data[10] & 0x02);
    input_report_key(idev, BTN_TRIGGER_HAPPY19, data[10] & 0x04);
    input_report_key(idev, BTN_TRIGGER_HAPPY20, data[10] & 0x08);
    input_report_key(idev, BTN_TRIGGER_HAPPY21, data[10] & 0x10);
    input_report_key(idev, BTN_TRIGGER_HAPPY22, data[10] & 0x20);
    input_report_key(idev, BTN_TRIGGER_HAPPY23, data[10] & 0x40);
    input_report_key(idev, BTN_TRIGGER_HAPPY24, data[10] & 0x80);

    input_report_key(idev, BTN_TRIGGER_HAPPY25, data[11] & 0x01);
    input_report_key(idev, BTN_TRIGGER_HAPPY26, data[11] & 0x02);
    input_report_key(idev, BTN_TRIGGER_HAPPY27, data[11] & 0x04);
    input_report_key(idev, BTN_TRIGGER_HAPPY28, data[11] & 0x08);
    input_report_key(idev, BTN_TRIGGER_HAPPY29, data[11] & 0x10);
    input_report_key(idev, BTN_TRIGGER_HAPPY30, data[11] & 0x20);
    input_report_key(idev, BTN_TRIGGER_HAPPY31, data[11] & 0x40);
    input_report_key(idev, BTN_TRIGGER_HAPPY32, data[11] & 0x80);

    input_report_key(idev, BTN_TRIGGER_HAPPY33, data[12] & 0x01);
    input_report_key(idev, BTN_TRIGGER_HAPPY34, data[12] & 0x02);
    input_report_key(idev, BTN_TRIGGER_HAPPY35, data[12] & 0x04);
    input_report_key(idev, BTN_TRIGGER_HAPPY36, data[12] & 0x08);
    input_report_key(idev, BTN_TRIGGER_HAPPY37, data[12] & 0x10);
    input_report_key(idev, BTN_TRIGGER_HAPPY38, data[12] & 0x20);
    input_report_key(idev, BTN_TRIGGER_HAPPY39, data[12] & 0x40);

    input_report_abs(idev, ABS_X, 
        (stick_axis >> X52PRO_X_SHIFT) & X52PRO_X_MASK);
    input_report_abs(idev, ABS_Y, 
        (stick_axis >> X52PRO_Y_SHIFT) & X52PRO_Y_MASK);
    input_report_abs(idev, ABS_RZ, 
        (stick_axis >> X52PRO_RZ_SHIFT) & X52PRO_RZ_MASK);

    input_report_abs(idev, ABS_THROTTLE, data[4]);
    input_report_abs(idev, ABS_RX, data[5]);
    input_report_abs(idev, ABS_RY, data[6]);
    input_report_abs(idev, ABS_Z, data[7]);

    input_report_abs(idev, ABS_TILT_X, data[14] & 0xF);
    input_report_abs(idev, ABS_TILT_Y, data[14] >> 4);

    switch(data[13]) {
    case 0x00:
        input_report_abs(idev, ABS_HAT0X, 0);
        input_report_abs(idev, ABS_HAT0Y, 0);
        break;
    case 0x10:
        input_report_abs(idev, ABS_HAT0X, 0);
        input_report_abs(idev, ABS_HAT0Y, -1);
        break;
    case 0x20:
        input_report_abs(idev, ABS_HAT0X, 1);
        input_report_abs(idev, ABS_HAT0Y, -1);
        break;
    case 0x30:
        input_report_abs(idev, ABS_HAT0X, 1);
        input_report_abs(idev, ABS_HAT0Y, 0);
        break;
    case 0x40:
        input_report_abs(idev, ABS_HAT0X, 1);
        input_report_abs(idev, ABS_HAT0Y, 1);
        break;
    case 0x50:
        input_report_abs(idev, ABS_HAT0X, 0);
        input_report_abs(idev, ABS_HAT0Y, 1);
        break;
    case 0x60:
        input_report_abs(idev, ABS_HAT0X, -1);
        input_report_abs(idev, ABS_HAT0Y, 1);
        break;
    case 0x70:
        input_report_abs(idev, ABS_HAT0X, -1);
        input_report_abs(idev, ABS_HAT0Y, 0);
        break;
    case 0x80:
        input_report_abs(idev, ABS_HAT0X, -1);
        input_report_abs(idev, ABS_HAT0Y, -1);
        break;
    }

    input_sync(idev);
}

void x52_irq_handler(struct urb *urb)
{
	struct x52_joy *joy = urb->context;
	int retval, status;

	status = urb->status;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* This URB is terminated, clean up */
		dbg("%s - URB shutting down with status: %d",
			__func__, status);
		return;
	default:
		dbg("%s - nonzero URB status received: %d",
			__func__, status);
		goto error;
	}

	switch (joy->type) {
	case X52TYPE_X52PRO:
        x52pro_decode_urb(joy, joy->idata);
		break;
	case X52TYPE_X52:
		break;
	default:
        break;
	}

error:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval)
		err ("%s - usb_submit_urb failed with result %d",
		     __func__, retval);
    
}

void x52_setup_input(struct input_dev *idev)
{
    int i;

    if (!idev) {
        return;
    }

    /*
     * Enable event inputs.
     *
     * EV_KEY for buttons (and keyboard events in the future)
     * EV_ABS for the axis
     * EV_REL for future mouse support by the mouse stick
     */
    idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);

    /* For now, map the buttons directly */
    for (i = BTN_TRIGGER_HAPPY1; i <= BTN_TRIGGER_HAPPY39; i++) {
        __set_bit(i, idev->keybit);
    }

    /* Map the axes */
    input_set_abs_params(idev, ABS_X, 0, 1023, 16, 512);
    input_set_abs_params(idev, ABS_Y, 0, 1023, 16, 512);
    input_set_abs_params(idev, ABS_RZ, 0, 1023, 16, 512);

    input_set_abs_params(idev, ABS_THROTTLE, 0, 255, 0, 0);
    input_set_abs_params(idev, ABS_RX, 0, 255, 16, 128);
    input_set_abs_params(idev, ABS_RY, 0, 255, 16, 128);
    input_set_abs_params(idev, ABS_Z, 0, 255, 0, 0);

    /* Mouse stick */
    input_set_abs_params(idev, ABS_TILT_X, 0, 15, 0, 8);
    input_set_abs_params(idev, ABS_TILT_Y, 0, 15, 0, 8);

    /* Hat switch */
    input_set_abs_params(idev, ABS_HAT0X, -1, 1, 0, 0);
    input_set_abs_params(idev, ABS_HAT0Y, -1, 1, 0, 0);
}

int x52_open (struct input_dev *idev)
{
    struct x52_joy *joy = input_get_drvdata(idev);
    
    joy->irq_in->dev = joy->udev;
    if (usb_submit_urb(joy->irq_in, GFP_KERNEL)) {
        return -EIO;
    }

    return 0;
}

void x52_close (struct input_dev *idev)
{
    struct x52_joy *joy = input_get_drvdata(idev);

    usb_kill_urb(joy->irq_in);
}

