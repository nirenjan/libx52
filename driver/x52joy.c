/*
 * Saitek X52 Pro HOTAS driver - 1.0
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

#define DRIVER_AUTHOR "Nirenjan Krishnan, nirenjan@gmail.com"
#define DRIVER_DESC "Saitek X52Pro HOTAS Driver"

#define VENDOR_ID_SAITEK    0x06a3
#define PRODUCT_ID_X52_PRO  0x0762

/* list of devices that work with this driver */
static struct usb_device_id id_table[] = {
    { USB_DEVICE(VENDOR_ID_SAITEK, PRODUCT_ID_X52_PRO) },
    /*
     * Future versions of this driver may support the original
     * X52 HOTAS joystick, but for now, only the X52 Pro is
     * supported.
     */
    { },
};
MODULE_DEVICE_TABLE (usb, id_table);

/*
 * The X52 MFD supports the following:
 *  - 3 lines of 16 characters each
 *  - Clock with HH:MM
 *  - Date with YYMMDD (IIRC)
 */
#define DRIVER_LINE_SIZE    256 /* Support upto 256 bytes in the driver */
#define X52_MFD_LINE_SIZE   16
#define X52_MFD_LINES       3

struct x52_mfd_line {
    u8      text[DRIVER_LINE_SIZE];
    u16     length;
    u16     current_pos;
};

struct x52_joy {
    struct usb_device   *udev;
    u32                 led_status;
    struct x52_mfd_line line[X52_MFD_LINES];
    u8                  time_hour;
    u8                  time_min;
    u16                 time_offs2;
    u16                 time_offs3;
    u8                  date_year;
    u8                  date_month;
    u8                  date_day;
    u8                  bri_mfd;
    u8                  bri_led;

    u8                  mode_h24:1;
    u8                  feat_mfd:1;
    u8                  feat_led:1;
    u8                  debug:1;
};

/**********************************************************************
 * MFD Line manipulation functions
 *********************************************************************/
static const u8 line_cmd[X52_MFD_LINES] = {
    X52_MFD_LINE1,
    X52_MFD_LINE2,
    X52_MFD_LINE3,
};

static void set_text(struct x52_joy *joy, u8 line_no)
{
    u16 i;
    u16 pos;
    u16 ch;
    u16 length;
    char *text_ptr;
    int retval;

    if (!joy || line_no >= X52_MFD_LINES) {
        /* Just some sanity checking */
        return;
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
        return;
    }
    pos = joy->line[line_no].current_pos;
    length = joy->line[line_no].length;

    for (i = 0; i < X52_MFD_LINE_SIZE; i+= 2) {
        text_ptr = &joy->line[line_no].text[pos];
        if (length - pos > 1) {
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

        pos += 2;

        if (pos >= length) {
            pos = 0;
            break;
        }
    }

    joy->line[line_no].current_pos = pos;
    return;
}

static ssize_t show_text_line(struct device *dev, char *buf, u8 line)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

    line--; /* Convert to 0-based line number */
    
    if (joy->feat_mfd) {
        return sprintf(buf, "%s\n", joy->line[line].text);
    } else {
        sprintf(buf, "\n");
        return -EOPNOTSUPP;
    }
    return 0;
}

static ssize_t set_text_line(struct device *dev, const char *buf,\
                             size_t count, u8 line)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);
    u16 i;
    u16 length;

    if (!joy->feat_mfd) {
        return -EOPNOTSUPP;
    }

    line--; /* Convert to 0-based line number */
    
    /* Copy the string from src to dst, upto 16 characters max */
    for (i = 0, length = 0; i < DRIVER_LINE_SIZE - 1 && i < count; i++, length++) {
        joy->line[line].text[i] = buf[i];
    }
    joy->line[line].length = length;
    joy->line[line].current_pos = 0;

    /* Append empty bytes until the destination is full */
    for ( ; i < DRIVER_LINE_SIZE; i++) {
        /* The X52 pro MFD uses space characters as empty characters */
        joy->line[line].text[i] = 32;
    }
    set_text(joy, line);
    return count;
}


#define show_set_text(no) \
static ssize_t show_text_line##no(struct device *dev, struct device_attribute *attr, char *buf)  \
{                                                               \
    return show_text_line(dev, buf, no);                        \
}                                                               \
static ssize_t set_text_line##no(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) \
{                                                               \
    return set_text_line(dev, buf, count, no);                  \
}                                                               \
static DEVICE_ATTR(line##no, S_IWUGO | S_IRUGO, show_text_line##no, set_text_line##no);

show_set_text(1);
show_set_text(2);
show_set_text(3);

/**********************************************************************
 * Brightness manipulation functions
 *********************************************************************/
#define mfd 0
#define led 1

static int to_hex(const char c)
{
    /* Converts a single character to hex */
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    }
    return -1;
}

static ssize_t set_brightness(struct device *dev, const char *buf,
                              size_t count, u8 target)
{
    u16 bri;
    u16 ch;
    int retval;
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

    if (target >= 2 || count != 4 || buf[0] != '0' || 
        ((buf[1] | 0x20) != 'x')) {
        /* Just some sanity checking */
        return -EOPNOTSUPP;
    }

    bri = 0;
    ch = to_hex(buf[2]);
    if (ch >= 0) {
        bri |= ch;
        bri <<= 4;
    } else {
        return -EOPNOTSUPP;
    }
    ch = to_hex(buf[3]);
    if (ch >= 0) {
        bri |= ch;
    } else {
        return -EOPNOTSUPP;
    }

    if (bri > 0x80) {
        return -EOPNOTSUPP;
    }

    if (target == mfd) {
        ch = X52_MFD_BRIGHTNESS;
        joy->bri_mfd = bri;
    } else {
        ch = X52_LED_BRIGHTNESS;
        joy->bri_led = bri;
    }

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                bri, ch,
                NULL, 0, 1000);
    return count;
}

static ssize_t show_brightness(struct device *dev, char *buf, u8 target)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

    if (target == mfd) {
        if (joy->feat_mfd) {
            return sprintf(buf, "0x%02x\n", joy->bri_mfd);
        }
    } else if (target == led) {
        if (joy->feat_led) {
            return sprintf(buf, "0x%02x\n", joy->bri_led);
        }
    }

    sprintf(buf, "\n");
    return -EOPNOTSUPP;
}

static ssize_t show_bri_mfd (struct device *dev, struct device_attribute *attr, char *buf)
{
    return show_brightness(dev, buf, mfd);
}
static ssize_t set_bri_mfd (struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return set_brightness(dev, buf, count, mfd);
}
static ssize_t show_bri_led (struct device *dev, struct device_attribute *attr, char *buf)
{
    return show_brightness(dev, buf, led);
}
static ssize_t set_bri_led (struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return set_brightness(dev, buf, count, led);
}
static DEVICE_ATTR(bri_mfd, S_IWUGO | S_IRUGO, show_bri_mfd, set_bri_mfd);
static DEVICE_ATTR(bri_led, S_IWUGO | S_IRUGO, show_bri_led, set_bri_led);

#undef mfd
#undef led

/**********************************************************************
 * LED manipulation functions
 *********************************************************************/

#define fire        X52_LED_FIRE
#define a_red       X52_LED_A_RED
#define a_green     X52_LED_A_GREEN
#define b_red       X52_LED_B_RED
#define b_green     X52_LED_B_GREEN
#define d_red       X52_LED_D_RED
#define d_green     X52_LED_D_GREEN
#define e_red       X52_LED_E_RED
#define e_green     X52_LED_E_GREEN
#define t1_red      X52_LED_T1_RED
#define t1_green    X52_LED_T1_GREEN
#define t2_red      X52_LED_T2_RED
#define t2_green    X52_LED_T2_GREEN
#define t3_red      X52_LED_T3_RED
#define t3_green    X52_LED_T3_GREEN
#define pov_red     X52_LED_POV_RED
#define pov_green   X52_LED_POV_GREEN
#define i_red       X52_LED_I_RED
#define i_green     X52_LED_I_GREEN
#define throttle    X52_LED_THROTTLE

/*
static int set_x52_led(struct x52_joy *joy, u8 target, u8 status)
{
    int retval;

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                target << 8 | status, X52_LED,
                NULL, 0, 1000);
    return retval;
}
*/

static ssize_t set_led(struct device *dev, const char *buf,
                              size_t count, u8 target)
{
    u8 status;
    int retval;
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

    if (target > X52_LED_THROTTLE || count < 1 || buf[0] < '0' || 
        buf[0] > '1') {
        /* Just some sanity checking */
        return -EOPNOTSUPP;
    }

    status = buf[0] - '0';

    retval = usb_control_msg(joy->udev,
                usb_sndctrlpipe(joy->udev, 0),
                X52_VENDOR_REQUEST,
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                target << 8 | status, X52_LED,
                NULL, 0, 1000);
    return count;
}

static ssize_t show_led(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

    if (joy->feat_led) {
        return sprintf(buf, "0x%08x\n", joy->led_status);
    }

    sprintf(buf, "\n");
    return -EOPNOTSUPP;
}

#define show_set_led(no)   \
static ssize_t set_led_##no(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) \
{                                                               \
    return set_led(dev, buf, count, no);                  \
}                                                               \
static DEVICE_ATTR(led_##no, S_IWUGO | S_IRUGO, show_led, set_led_##no);

show_set_led(fire);
show_set_led(a_red);
show_set_led(a_green);
show_set_led(b_red);
show_set_led(b_green);
show_set_led(d_red);
show_set_led(d_green);
show_set_led(e_red);
show_set_led(e_green);
show_set_led(t1_red);
show_set_led(t1_green);
show_set_led(t2_red);
show_set_led(t2_green);
show_set_led(t3_red);
show_set_led(t3_green);
show_set_led(pov_red);
show_set_led(pov_green);
show_set_led(i_red);
show_set_led(i_green);
show_set_led(throttle);

/**********************************************************************
 * X52 driver functions
 *********************************************************************/
static int x52_probe(struct usb_interface *intf,
                     const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(intf);
    struct x52_joy *joy = NULL;
    int retval = -ENOMEM;

    joy = kmalloc(sizeof(*joy), GFP_KERNEL);
    if (joy == NULL) {
        dev_err(&intf->dev, "Out of memory\n");
        goto error;
    }
    memset(joy, 0, sizeof(*joy));

    joy->udev = usb_get_dev(dev);

    /* Set the feature bits */
    joy->feat_mfd = 1;
    joy->feat_led = 1;

    usb_set_intfdata(intf, joy);

    device_create_file(&intf->dev, &dev_attr_line1);
    device_create_file(&intf->dev, &dev_attr_line2);
    device_create_file(&intf->dev, &dev_attr_line3);

    device_create_file(&intf->dev, &dev_attr_bri_mfd);
    device_create_file(&intf->dev, &dev_attr_bri_led);

    device_create_file(&intf->dev, &dev_attr_led_fire);
    device_create_file(&intf->dev, &dev_attr_led_a_red);
    device_create_file(&intf->dev, &dev_attr_led_a_green);
    device_create_file(&intf->dev, &dev_attr_led_b_red);
    device_create_file(&intf->dev, &dev_attr_led_b_green);
    device_create_file(&intf->dev, &dev_attr_led_d_red);
    device_create_file(&intf->dev, &dev_attr_led_d_green);
    device_create_file(&intf->dev, &dev_attr_led_e_red);
    device_create_file(&intf->dev, &dev_attr_led_e_green);
    device_create_file(&intf->dev, &dev_attr_led_t1_red);
    device_create_file(&intf->dev, &dev_attr_led_t1_green);
    device_create_file(&intf->dev, &dev_attr_led_t2_red);
    device_create_file(&intf->dev, &dev_attr_led_t2_green);
    device_create_file(&intf->dev, &dev_attr_led_t3_red);
    device_create_file(&intf->dev, &dev_attr_led_t3_green);
    device_create_file(&intf->dev, &dev_attr_led_pov_red);
    device_create_file(&intf->dev, &dev_attr_led_pov_green);
    device_create_file(&intf->dev, &dev_attr_led_i_red);
    device_create_file(&intf->dev, &dev_attr_led_i_green);
    device_create_file(&intf->dev, &dev_attr_led_throttle);

    dev_info(&intf->dev, "X52 device now attached\n");
    return 0;

error:
    kfree(joy);
    return retval;
}

static void x52_disconnect(struct usb_interface *intf)
{
    struct x52_joy *joy;

    joy = usb_get_intfdata(intf);
    usb_set_intfdata(intf, NULL);

    device_remove_file(&intf->dev, &dev_attr_line1);
    device_remove_file(&intf->dev, &dev_attr_line2);
    device_remove_file(&intf->dev, &dev_attr_line3);

    usb_put_dev(joy->udev);
    kfree(joy);

    dev_info(&intf->dev, "X52 device now disconnected\n");
}

static struct usb_driver x52_driver = {
    .name = "X52",
    .probe = x52_probe,
    .disconnect = x52_disconnect,
    .id_table = id_table,
};

static int __init x52_init(void)
{
    int retval = 0;

    retval = usb_register(&x52_driver);
    if (retval) {
        err("usb_register failed (errno=%d)\n", retval);
    }

    return retval;
}

static void __exit x52_exit(void)
{
    usb_deregister(&x52_driver);
}

module_init (x52_init);
module_exit (x52_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL v2");

