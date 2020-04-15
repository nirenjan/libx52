/*
 * LibUSB stub driver for testing the Saitek X52/X52 Pro
 *
 * Copyright (C) 2017 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <pthread.h>
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

    // Hotplug support
    int hotplug_vid;
    int hotplug_pid;
    libusb_hotplug_event events;
    libusb_hotplug_callback_fn callback;
    void * cb_user_data;

    // Hotplug threading
    volatile int stop_thread;
    pthread_t hotplug_pthread;
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

/**
 * @brief Device update FIFO environment variable
 *
 * This is used by the test driver to update the simulated USB device list
 */
#define INPUT_DEVICE_FIFO_ENV           "LIBUSBX52_DEVICE_FIFO"

/**
 * @brief Default file location of the device update FIFO
 *
 * This FIFO is read by a thread in libusbx52 and used to simulate the hotplug
 * functionality of libusb.
 */
#define DEFAULT_INPUT_DEVICE_FIFO_FILE  "/tmp/libusbx52_device_fifo"

libusb_device* vector_push(libusb_context *ctx, int vid, int pid);
libusb_device* vector_pop(libusb_context *ctx, int vid, int pid);
