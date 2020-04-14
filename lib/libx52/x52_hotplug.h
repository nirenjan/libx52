/*
 * Saitex X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2020 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52_HOTPLUG_H
#define X52_HOTPLUG_H

#include <stdint.h>
#include <stdbool.h>
#include <libusb.h>
#include "libx52.h"

/* Callback function for hotplug events */
typedef void (*libx52_hotplug_fn)(bool inserted, void *user_data, libx52_device *dev);

/*
 * Structure for callback handle. This is a node in a doubly linked list,
 * which is iterated over by the libusb callback handler.
 */
struct libx52_hotplug_callback_handle {
    libx52_hotplug_service *svc;

    libx52_hotplug_fn callback;
    void *user_data;

    size_t id;
};

struct libx52_hotplug_service {
    libx52_device *dev;

    libusb_hotplug_callback_handle cb_handle;

    libx52_hotplug_callback_handle **callbacks;
    size_t num_callbacks;
};

#define DEFAULT_NUM_CALLBACKS 8

#endif /* !defined X52_HOTPLUG_H */
