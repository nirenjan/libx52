/*
 * LibUSB stub driver for testing the Saitek X52/X52 Pro
 *
 * Copyright (C) 2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2.
 *
 */

#include <stdio.h>
// #include <libusb-1.0/libusb.h>
#include <libusb.h>

struct libusb_device {
    struct libusb_context *context;
    int index;
    int ref_count;
    struct libusb_device_descriptor desc;
};

struct libusb_context {
    int block_size;     // Set to LIBUSBX52_MEMORY_BLOCK_SIZE
    int max_devices;    // Calculated based on block_size
    int debug_level;
    int num_devices;
    struct libusb_device *devices;
};

struct libusb_device_handle {
    struct libusb_context *ctx;
    struct libusb_device *dev;
    int packets_written;
    FILE *packet_data_file;
};

/**
 * @brief Device list file environment variable
 *
 * This is used by the test driver to create a temporary environment for
 * the device list
 */
#define INPUT_DEVICE_LIST_ENV           "LIBUSBX52_DEVICE_LIST"

/**
 * @brief Default file location of the device list file
 *
 * This file contains a list of VIDs and PIDs in hexadecimal format separated
 * by spaces. There must be an even number of entries, each pair corresponding
 * to a (VID, PID) tuple identifying a single USB device.
 */
#define DEFAULT_INPUT_DEVICE_LIST_FILE  "/tmp/libusbx52_device_list"

/**
 * @brief Output data environment variable
 *
 * This is used by the test driver to create a temporary environment for
 * the output data
 */
#define OUTPUT_DATA_FILE_ENV            "LIBUSBX52_OUTPUT_DATA"

/**
 * @brief Default file location of the communication data file
 *
 * This file contains the libusb APIs called by libx52, after a device has
 * been opened, i.e., all APIs that operate on a libusb_device_handle
 */
#define DEFAULT_OUTPUT_DATA_FILE        "/tmp/libusbx52_output_data"

