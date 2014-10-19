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
#include <linux/input.h>
#include <linux/usb/input.h>

#include "x52joy_commands.h"
#include "x52joy_common.h"

#define DRIVER_AUTHOR "Nirenjan Krishnan, nirenjan@gmail.com"
#define DRIVER_DESC "Saitek X52Pro HOTAS Driver"
#define DRIVER_VERSION "1.0"

static const struct x52_device {
    u16     idVendor;
    u16     idProduct;
    char    *name;
    u8      type;
    u8      flags;
} x52_devices[] = {
    {
        VENDOR_ID_SAITEK, PRODUCT_ID_X52_PRO,
        "Saitek X52 Pro Flight Control System", X52TYPE_X52PRO,
        X52FLAGS_SUPPORTS_MFD | X52FLAGS_SUPPORTS_LED
    },
    {   /* Terminating entry */
        0x0000, 0x0000, "Unknown X52", X52TYPE_UNKNOWN, 0
    },
};

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

#define CHECK_RETURN(ret, cnt) do { \
    if (ret) {\
        return ret;\
    } else {\
        return cnt;\
    }\
} while(0)

/**********************************************************************
 * MFD Line manipulation functions
 *********************************************************************/
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

static ssize_t set_text_line(struct device *dev, const char *buf,
                             size_t count, u8 line)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);
    u16 i;
    u16 length;
    int retval;

    if (!joy->feat_mfd) {
        return -EOPNOTSUPP;
    }

    line--; /* Convert to 0-based line number */
    
    /* Copy the string from src to dst, upto 16 characters max */
    for (i = 0, length = 0; i < X52_MFD_LINE_SIZE && i < count; i++, length++) {
        joy->line[line].text[i] = buf[i];
    }
    joy->line[line].length = length;

    /* Append empty bytes until the destination is full */
    for ( ; i < X52_MFD_LINE_SIZE; i++) {
        /* The X52 pro MFD uses space characters as empty characters */
        joy->line[line].text[i] = 32;
    }

    retval = set_text(joy, line);

    CHECK_RETURN(retval, count);
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
static DEVICE_ATTR(mfd_line##no, S_IWUGO | S_IRUGO, show_text_line##no, set_text_line##no);

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

static ssize_t set_x52_brightness(struct device *dev, const char *buf,
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

    retval = set_brightness(joy, target);

    CHECK_RETURN(retval, count);
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
    return set_x52_brightness(dev, buf, count, mfd);
}
static ssize_t show_bri_led (struct device *dev, struct device_attribute *attr, char *buf)
{
    return show_brightness(dev, buf, led);
}
static ssize_t set_bri_led (struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return set_x52_brightness(dev, buf, count, led);
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

static ssize_t set_x52_led(struct device *dev, const char *buf,
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

    if (status) {
        joy->led_status |= (1 << target);
    } else {
        joy->led_status &= ~(1 << target);
    }

    retval = set_led(joy, target);

    CHECK_RETURN(retval, count);
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
    return set_x52_led(dev, buf, count, no);                  \
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
 * Blink manipulation functions
 *********************************************************************/
static ssize_t show_blink(struct device *dev, struct device_attribute *attr,
                          char *buf)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);

    if (joy->feat_led) {
        return sprintf(buf, "%d\n", joy->blink_led);
    } else {
        sprintf(buf, "\n");
        return -EOPNOTSUPP;
    }
    return 0;
}

static ssize_t set_x52_blink(struct device *dev, struct device_attribute *attr,
                         const char *buf, size_t count)
{
    struct usb_interface *intf = to_usb_interface(dev);
    struct x52_joy *joy = usb_get_intfdata(intf);
    int retval;

    if (!joy->feat_led) {
        return -EOPNOTSUPP;
    }

    if (count < 1 || buf[0] < '0' || buf[1] > '1') {
        return -EINVAL;
    }

    joy->blink_led = buf[0] - '0';

    retval = set_blink(joy);

    CHECK_RETURN(retval, count);
}


static DEVICE_ATTR(led_blink, S_IWUGO | S_IRUGO, show_blink, set_x52_blink);

/**********************************************************************
 * X52 driver functions
 *********************************************************************/
static int x52_probe(struct usb_interface *intf,
                     const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(intf);
    struct x52_joy *joy = NULL;
    struct input_dev *idev = NULL;
    struct usb_endpoint_descriptor *ep_irq_in;
    int retval = -ENOMEM;
    int i;

    for (i = 0; x52_devices[i].idVendor; i++) {
        if ((le16_to_cpu(udev->descriptor.idVendor) == x52_devices[i].idVendor) &&
            (le16_to_cpu(udev->descriptor.idProduct) == x52_devices[i].idProduct)) {
                break;
            }
    }

    joy = kzalloc(sizeof(*joy), GFP_KERNEL);
    if (joy == NULL) {
        dev_err(&intf->dev, "Out of memory: Cannot create joystick\n");
        goto error;
    }
    joy->type = x52_devices[i].type;

    idev = input_allocate_device();
    if (idev == NULL) {
        dev_err(&intf->dev, "Out of memory: Cannot create input\n");
        goto error;
    }

    joy->idata = usb_alloc_coherent(udev, X52_PACKET_LEN, GFP_KERNEL,
                                    &joy->idata_dma);
    if (!joy->idata) {
        dev_err(&intf->dev, "Out of memory: Cannot alloc coherent buffer\n");
        goto error;
    }

    joy->irq_in = usb_alloc_urb(0, GFP_KERNEL);
    if (!joy->irq_in) {
        dev_err(&intf->dev, "Out of memory: Cannot alloc IRQ URB\n");
        goto error1;
    }

    joy->udev = usb_get_dev(udev);

    joy->idev = idev;
    usb_make_path(udev, joy->phys, sizeof(joy->phys));
    strlcat(joy->phys, "/input0", sizeof(joy->phys));

    idev->name = x52_devices[i].name;
    idev->phys = joy->phys;
    usb_to_input_id(udev, &idev->id);
    idev->dev.parent = &intf->dev;
    
    input_set_drvdata(idev, joy);
    
    idev->open = x52_open;
    idev->close = x52_close;

    x52_setup_input(idev);

    ep_irq_in = &intf->cur_altsetting->endpoint[0].desc;
    usb_fill_int_urb(joy->irq_in, udev,
                     usb_rcvintpipe(udev, ep_irq_in->bEndpointAddress),
                     joy->idata, X52_PACKET_LEN, x52_irq_handler,
                     joy, ep_irq_in->bInterval);
	joy->irq_in->transfer_dma = joy->idata_dma;
	joy->irq_in->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    retval = input_register_device(joy->idev);
    if (retval) {
        goto error2;
    }

    /* Set the feature bits */
    joy->feat_mfd = !!(x52_devices[i].flags & X52FLAGS_SUPPORTS_MFD);
    joy->feat_led = !!(x52_devices[i].flags & X52FLAGS_SUPPORTS_LED);

    usb_set_intfdata(intf, joy);

    if (joy->feat_mfd) {
        device_create_file(&intf->dev, &dev_attr_mfd_line1);
        device_create_file(&intf->dev, &dev_attr_mfd_line2);
        device_create_file(&intf->dev, &dev_attr_mfd_line3);

        device_create_file(&intf->dev, &dev_attr_bri_mfd);
    }

    if (joy->feat_led) {
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

        device_create_file(&intf->dev, &dev_attr_led_blink);
    }

    try_module_get(THIS_MODULE);

    dev_info(&intf->dev, "X52 device now attached\n");
    return 0;

error2:
    usb_free_urb(joy->irq_in);
error1:
    usb_free_coherent(udev, X52_PACKET_LEN, joy->idata, joy->idata_dma);
error:
    input_free_device(idev);
    kfree(joy);
    return retval;
}

static void x52_disconnect(struct usb_interface *intf)
{
    struct x52_joy *joy;

    joy = usb_get_intfdata(intf);
    usb_set_intfdata(intf, NULL);

    usb_kill_urb(joy->irq_in);
    usb_free_urb(joy->irq_in);
    usb_free_coherent(joy->udev, X52_PACKET_LEN, joy->idata, joy->idata_dma);
    input_free_device(joy->idev);

    if (joy->feat_mfd) {
        device_remove_file(&intf->dev, &dev_attr_mfd_line1);
        device_remove_file(&intf->dev, &dev_attr_mfd_line2);
        device_remove_file(&intf->dev, &dev_attr_mfd_line3);

        device_remove_file(&intf->dev, &dev_attr_bri_mfd);
    }

    if (joy->feat_led) {
        device_remove_file(&intf->dev, &dev_attr_bri_led);

        device_remove_file(&intf->dev, &dev_attr_led_fire);
        device_remove_file(&intf->dev, &dev_attr_led_a_red);
        device_remove_file(&intf->dev, &dev_attr_led_a_green);
        device_remove_file(&intf->dev, &dev_attr_led_b_red);
        device_remove_file(&intf->dev, &dev_attr_led_b_green);
        device_remove_file(&intf->dev, &dev_attr_led_d_red);
        device_remove_file(&intf->dev, &dev_attr_led_d_green);
        device_remove_file(&intf->dev, &dev_attr_led_e_red);
        device_remove_file(&intf->dev, &dev_attr_led_e_green);
        device_remove_file(&intf->dev, &dev_attr_led_t1_red);
        device_remove_file(&intf->dev, &dev_attr_led_t1_green);
        device_remove_file(&intf->dev, &dev_attr_led_t2_red);
        device_remove_file(&intf->dev, &dev_attr_led_t2_green);
        device_remove_file(&intf->dev, &dev_attr_led_t3_red);
        device_remove_file(&intf->dev, &dev_attr_led_t3_green);
        device_remove_file(&intf->dev, &dev_attr_led_pov_red);
        device_remove_file(&intf->dev, &dev_attr_led_pov_green);
        device_remove_file(&intf->dev, &dev_attr_led_i_red);
        device_remove_file(&intf->dev, &dev_attr_led_i_green);
        device_remove_file(&intf->dev, &dev_attr_led_throttle);

        device_remove_file(&intf->dev, &dev_attr_led_blink);
    }

    usb_put_dev(joy->udev);
    kfree(joy);

    module_put(THIS_MODULE);

    dev_info(&intf->dev, "X52 device now disconnected\n");
}

static struct usb_driver x52_driver = {
    .name = "saitek_x52",
    .probe = x52_probe,
    .disconnect = x52_disconnect,
    .id_table = id_table,
};

static int __init x52_init(void)
{
    int retval = 0;

    retval = usb_register(&x52_driver);
    if (retval) {
        pr_err("usb_register failed (errno=%d)\n", retval);
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
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL v2");

