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
#define X52_MFD_LINE_SIZE   17 /* Add one for null byte */

struct x52_mfd_line {
    u8      text[DRIVER_LINE_SIZE];
    u16     length;
    u16     current_pos;
};

struct x52_joy {
    struct usb_device   *udev;
    u32                 led_status;
    struct x52_mfd_line line[3];
    u8                  time_hour;
    u8                  time_min;
    u16                 time_offs2;
    u16                 time_offs3;
    u8                  date_year;
    u8                  date_month;
    u8                  date_day;

    u8                  mode_h24:1;
    u8                  feat_mfd:1;
    u8                  feat_led:1;
    u8                  debug:1;
};

/**********************************************************************
 * MFD Line manipulation functions
 *********************************************************************/
static void set_text(u8 line_no)
{
    /* TODO */
    return;
}

static ssize_t show_text_line(struct device *dev, char *buf, u8 line)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

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
    set_text(line);
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

