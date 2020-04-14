/*
 * Saitek X52 Pro MFD & LED driver
 *
 * Copyright (C) 2012-2015 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "libx52.h"
#include "x52_commands.h"
#include "x52_common.h"
#include "x52_hotplug.h"

#define VENDOR_SAITEK 0x06a3
#define X52_PROD_X52PRO 0x0762
#define X52_PROD_X52_1  0x0255
#define X52_PROD_X52_2  0x075C

/* Check if the USB device is supported by this library */
static int libx52_check_product(uint16_t idVendor, uint16_t idProduct)
{
    if (idVendor == VENDOR_SAITEK) {
        switch (idProduct) {
        case X52_PROD_X52_1:
        case X52_PROD_X52_2:
        case X52_PROD_X52PRO:
            return 1;
        }
    }

    return 0;
}

/* Set flags according to the device model */
static void libx52_set_dev_flags(libx52_device *dev, uint16_t idProduct)
{
    if (idProduct == X52_PROD_X52PRO) {
        set_bit(&(dev->flags), X52_FLAG_IS_PRO);
    }
}

int libx52_init(libx52_device **dev)
{
    int rc;
    ssize_t count;
    int i;
    libusb_device **list;
    libusb_device *device;
    libusb_device_handle *hdl;
    struct libusb_device_descriptor desc;
    libx52_device *x52_dev;

    /* Make sure that we have a valid return pointer */
    if (dev == NULL) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Allocate memory for the library's data structures */
    x52_dev = calloc(1, sizeof(libx52_device));
    if (!x52_dev) {
        return LIBX52_ERROR_OUT_OF_MEMORY;
    }

    rc = libusb_init(&(x52_dev->ctx));
    if (rc) {
        rc = LIBX52_ERROR_INIT_FAILURE;
        goto err_recovery;
    }
    #if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106)
    /*
     * Use the libusb_set_option flag instead of libusb_set_debug. This
     * was introduced in libusb 1.0.22
     */
    libusb_set_option(x52_dev->ctx, LIBUSB_OPTION_LOG_LEVEL,
                      LIBUSB_LOG_LEVEL_WARNING);
    #else
    libusb_set_debug(x52_dev->ctx, LIBUSB_LOG_LEVEL_WARNING);
    #endif

    count = libusb_get_device_list(x52_dev->ctx, &list);
    for (i = 0; i < count; i++) {
        device = list[i];
        if (!libusb_get_device_descriptor(device, &desc)) {
            if (libx52_check_product(desc.idVendor, desc.idProduct)) {
                rc = libusb_open(device, &hdl);
                if (rc) {
                    rc = libx52internal_translate_libusb_error(rc);
                    goto err_recovery;
                }

                x52_dev->hdl = hdl;

                libx52_set_dev_flags(x52_dev, desc.idProduct);
                break;
            }
        }
    }
    libusb_free_device_list(list, 1);

    /* Make sure we actually have an X52 device detected */
    if (!x52_dev->hdl) {
        rc = LIBX52_ERROR_NO_DEVICE;
        goto err_recovery;
    }

    *dev = x52_dev;
    return LIBX52_SUCCESS;

err_recovery:
    free(x52_dev);
    return rc;
}

void libx52_exit(libx52_device *dev)
{
    libusb_close(dev->hdl);
    libusb_exit(dev->ctx);
    free(dev);
}

static int _hotplug_handler(
    libusb_context *ctx,
    libusb_device *dev,
    libusb_hotplug_event event,
    void *user_data
) {
    libx52_hotplug_service *svc = user_data;
    libx52_hotplug_callback_handle *cb;
    struct libusb_device_descriptor desc;
    size_t i;
    int rc;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (rc != LIBUSB_SUCCESS) {
        return rc;
    }

    if (libx52_check_product(desc.idVendor, desc.idProduct)) {
        return LIBX52_ERROR_NOT_SUPPORTED;
    }

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        rc = libusb_open(dev, &(svc->dev->hdl));
        if (rc != LIBUSB_SUCCESS) {
            return libx52internal_translate_libusb_error(rc);
        }

        libx52_set_dev_flags(svc->dev, desc.idProduct);
    } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        if (svc->dev->hdl != NULL) {
            libusb_close(svc->dev->hdl);
            svc->dev->hdl = NULL;
        }
    } else {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Iterate through registered callbacks */
    for (i = 0; i < svc->num_callbacks; i++) {
        cb = svc->callbacks[i];
        if (cb->callback != NULL) {
            (cb->callback)(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                           cb->user_data, svc->dev);
        }
    }

    return 0;
}

int libx52_hotplug_init(libx52_hotplug_service **service)
{
    libx52_hotplug_service *svc;
    int rc = LIBX52_SUCCESS;

    /* Make sure that we have a valid return pointer */
    if (service == NULL) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Allocate memory for the library's data structures */
    svc = calloc(1, sizeof(*svc));
    if (svc == NULL) {
        rc = LIBX52_ERROR_OUT_OF_MEMORY;
        goto err_recovery;
    }

    /* Allocate initial buffer for callbacks list */
    svc->callbacks = calloc(DEFAULT_NUM_CALLBACKS, sizeof(*(svc->callbacks)));
    if (svc->callbacks == NULL) {
        rc = LIBX52_ERROR_OUT_OF_MEMORY;
        goto err_recovery;
    }
    svc->num_callbacks = DEFAULT_NUM_CALLBACKS;

    svc->dev = calloc(1, sizeof(*(svc->dev)));
    if (svc->dev == NULL) {
        rc = LIBX52_ERROR_OUT_OF_MEMORY;
        goto err_recovery;
    }

    /* Initialize libusb */
    rc = libusb_init(&(svc->dev->ctx));
    if (rc != LIBUSB_SUCCESS) {
        rc = LIBX52_ERROR_INIT_FAILURE;
        goto err_recovery;
    }
    #if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106)
    /*
     * Use the libusb_set_option flag instead of libusb_set_debug. This
     * was introduced in libusb 1.0.22
     */
    libusb_set_option(svc->dev->ctx, LIBUSB_OPTION_LOG_LEVEL,
                      LIBUSB_LOG_LEVEL_WARNING);
    #else
    libusb_set_debug(svc->dev->ctx, LIBUSB_LOG_LEVEL_WARNING);
    #endif

    /* Register with the libusb hotplug API */
    rc = libusb_hotplug_register_callback(svc->dev->ctx,
            LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
            LIBUSB_HOTPLUG_ENUMERATE, VENDOR_SAITEK,
            LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
            _hotplug_handler, svc, &(svc->cb_handle));
    if (rc != LIBUSB_SUCCESS) {
        rc = LIBX52_ERROR_INIT_FAILURE;
        libusb_exit(svc->dev->ctx);
        goto err_recovery;
    }

    *service = svc;
    return rc;

err_recovery:
    if (svc->dev != NULL) {
        free(svc->dev);
    }

    if (svc->callbacks != NULL) {
        free(svc->callbacks);
    }

    free(svc);
    return rc;
}

int libx52_hotplug_exit(libx52_hotplug_service *svc) {
    if (svc == NULL || svc->dev == NULL || svc->callbacks == NULL) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Deregister the callback handle */
    libusb_hotplug_deregister_callback(svc->dev->ctx, svc->cb_handle);

    /* Clean up registered callback handlers */
    for (size_t i = 0; i < svc->num_callbacks; i++) {
        free(svc->callbacks[i]);
    }
    free(svc->callbacks);

    if (svc->dev->hdl != NULL) {
        libusb_close(svc->dev->hdl);
    }

    libusb_exit(svc->dev->ctx);

    /* Clear the device area and free it */
    memset(svc->dev, 0, sizeof(*svc->dev));
    free(svc->dev);

    /* Clear the service pointer before freeing it */
    memset(svc, 0, sizeof(*svc));
    free(svc);

    return LIBX52_SUCCESS;
}

int libx52_hotplug_register_callback(libx52_hotplug_service *svc,
                                     libx52_hotplug_fn callback_fn,
                                     void *user_data,
                                     libx52_hotplug_callback_handle **cb_handle)
{
    libx52_hotplug_callback_handle *hdl;
    libx52_hotplug_callback_handle **callbacks;
    size_t i;

    if (svc == NULL || callback_fn == NULL) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Find an empty slot */
    for (i = 0; i < svc->num_callbacks; i++) {
        if (svc->callbacks[i] == NULL) {
            break;
        }
    }
    if (i == svc->num_callbacks) {
        /* Existing callbacks array is full, create a new one */
        callbacks = calloc(svc->num_callbacks + DEFAULT_NUM_CALLBACKS, sizeof(*callbacks));
        if (callbacks == NULL) {
            return LIBX52_ERROR_OUT_OF_MEMORY;
        }

        /* Copy the callbacks array to the new one */
        for (i = 0; i < svc->num_callbacks; i++) {
            callbacks[i] = svc->callbacks[i];
        }

        /* Free the old array, and adjust the link to the new larger array */
        svc->num_callbacks += DEFAULT_NUM_CALLBACKS;
        free(svc->callbacks);
        svc->callbacks = callbacks;
    }

    hdl = calloc(1, sizeof(*hdl));
    if (hdl == NULL) {
        return LIBX52_ERROR_OUT_OF_MEMORY;
    }

    hdl->svc = svc;
    hdl->id = i;
    hdl->callback = callback_fn;
    hdl->user_data = user_data;

    /* Insert the node into the callbacks list */
    svc->callbacks[i] = hdl;

    *cb_handle = hdl;
    return LIBX52_SUCCESS;
}

int libx52_hotplug_deregister_callback(libx52_hotplug_callback_handle *hdl)
{
    libx52_hotplug_service *svc;

    if (hdl == NULL || hdl->svc == NULL) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    svc = hdl->svc;

    if (hdl->id >= svc->num_callbacks) {
        return LIBX52_ERROR_INVALID_PARAM;
    }

    /* Free the handle */
    svc->callbacks[hdl->id] = NULL;
    free(hdl);

    return LIBX52_SUCCESS;
}
