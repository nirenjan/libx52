/*
 * HID driver for Saitek X65 HOTAS
 *
 * Copyright (c) 2021 Nirenjan Krishnan
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <linux/hid.h>
#include <linux/module.h>
#include <linux/bits.h>

#define VENDOR_SAITEK 0x06a3
#define DEV_X65F 0x0b6a

/* X65 Report format assuming the below is one long little-endian integer
 *
 * |-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
 *       Rz        |           Y           |          X            |
 * |-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
 *    Slider     |      Ry       |      Rx       |       Z       |
 * |-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
 *                  Buttons 1-31                                 |
 * |-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
 * | |MouseY |MouseX | Hat   |     Buttons 32-50
 * |-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
 */
static void _parse_axis_report(struct input_dev *input_dev, u8 *data)
{
    static const s32 hat_to_axis[16][2] = {
        {0, 0},
        {0, -1},
        {1, -1},
        {1, 0},
        {1, 1},
        {0, 1},
        {-1, 1},
        {-1, 0},
        {-1, -1},
    };

    u8 hat = (data[14] >> 3) & 0x0f;

    u32 axis = (data[2] << 16) |
               (data[1] <<  8) |
               data[0];

    input_report_abs(input_dev, ABS_X, (axis & 0xfff));
    input_report_abs(input_dev, ABS_Y, ((axis >> 12) & 0xfff));

    input_report_abs(input_dev, ABS_RZ, ((data[4] & 0x1) << 8) | data[3]);
    input_report_abs(input_dev, ABS_Z, ((data[5] & 0x1) << 8) | data[4]);
    input_report_abs(input_dev, ABS_RX, ((data[6] & 0x1) << 8) | data[5]);
    input_report_abs(input_dev, ABS_RY, ((data[7] & 0x1) << 8) | data[6]);
    input_report_abs(input_dev, ABS_THROTTLE, ((data[8] & 0x1) << 8) | data[7]);

    input_report_abs(input_dev, ABS_MISC, ((data[15] & 0x7) << 1) | ((data[14] & 0x80) >> 7));
    input_report_abs(input_dev, ABS_MISC + 1, (data[15] >> 3) & 0xf);

    input_report_abs(input_dev, ABS_HAT0X, hat_to_axis[hat][0]);
    input_report_abs(input_dev, ABS_HAT0Y, hat_to_axis[hat][1]);
}

static void _parse_button_report(struct input_dev *input_dev, u8 *data)
{
    u64 buttons;
    int report_button;
    int i;

    buttons = ((u64)data[14] << 47) |
              ((u64)data[13] << 39) |
              ((u64)data[12] << 31) |
              ((u64)data[11] << 23) |
              ((u64)data[10] << 15) |
              ((u64)data[9] << 7) |
              ((u64)data[8] >> 1);

    for (i = 0; i < 50; i++) {
        if (i <= 0xf) {
            report_button = BTN_JOYSTICK + i;
        } else {
            report_button = BTN_TRIGGER_HAPPY + i - 0x10;
        }
        input_report_key(input_dev, report_button, !!(buttons & (1UL << i)));
    }
}

static int x65_raw_event(struct hid_device *dev,
                         struct hid_report *report, u8 *data, int len)
{
    struct input_dev *input_dev = hid_get_drvdata(dev);

    _parse_axis_report(input_dev, data);
    _parse_button_report(input_dev, data);
    input_sync(input_dev);
    return 0;
}

static int x65_input_configured(struct hid_device *dev,
                                struct hid_input *input)
{
    struct input_dev *input_dev = input->input;
    int i;

    hid_set_drvdata(dev, input_dev);

    set_bit(EV_KEY, input_dev->evbit);
    set_bit(EV_ABS, input_dev->evbit);

    for (i = 0; i < 0x10; i++) {
        set_bit(BTN_JOYSTICK + i, input_dev->keybit);
    }
    for (i = 0x10; i < 50; i++) {
        set_bit(BTN_TRIGGER_HAPPY + i - 0x10, input_dev->keybit);
    }

    set_bit(ABS_X, input_dev->absbit);
    set_bit(ABS_Y, input_dev->absbit);
    set_bit(ABS_Z, input_dev->absbit);
    set_bit(ABS_RX, input_dev->absbit);
    set_bit(ABS_RY, input_dev->absbit);
    set_bit(ABS_RZ, input_dev->absbit);
    set_bit(ABS_THROTTLE, input_dev->absbit);
    set_bit(ABS_HAT0X, input_dev->absbit);
    set_bit(ABS_HAT0Y, input_dev->absbit);
    set_bit(ABS_MISC, input_dev->absbit);
    set_bit(ABS_MISC+1, input_dev->absbit);

    input_set_abs_params(input_dev, ABS_X, 0, 4095, 15, 255);
    input_set_abs_params(input_dev, ABS_Y, 0, 4095, 15, 255);
    input_set_abs_params(input_dev, ABS_RZ, 0, 511, 1, 31);
    input_set_abs_params(input_dev, ABS_RX, 0, 255, 0, 15);
    input_set_abs_params(input_dev, ABS_RY, 0, 255, 0, 15);
    input_set_abs_params(input_dev, ABS_Z, 0, 255, 0, 15);
    input_set_abs_params(input_dev, ABS_THROTTLE, 0, 255, 0, 15);
    input_set_abs_params(input_dev, ABS_HAT0X, -1, 1, 0, 0);
    input_set_abs_params(input_dev, ABS_HAT0Y, -1, 1, 0, 0);
    input_set_abs_params(input_dev, ABS_MISC, 0, 15, 0, 0);
    input_set_abs_params(input_dev, ABS_MISC+1, 0, 15, 0, 0);

    return 0;
}

static int x65_input_mapping(struct hid_device *dev,
                             struct hid_input *input,
                             struct hid_field *field,
                             struct hid_usage *usage,
                             unsigned long **bit,
                             int *max)
{
    /*
     * We are reporting the events in x65_raw_event.
     * Skip the hid-input processing.
     */
    return -1;
}

static const struct hid_device_id x65_devices[] = {
    { HID_USB_DEVICE(VENDOR_SAITEK, DEV_X65F) },
    {}
};

MODULE_DEVICE_TABLE(hid, x65_devices);

static struct hid_driver x65_driver = {
    .name = "saitek-x65",
    .id_table = x65_devices,
    .input_mapping = x65_input_mapping,
    .input_configured = x65_input_configured,
    .raw_event = x65_raw_event,
};

module_hid_driver(x65_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Nirenjan Krishnan");
MODULE_DESCRIPTION("HID driver for Saitek X65 HOTAS devices");
