/*
 * VKM stub implementation
 *
 * Copyright (C) 2026 Nirenjan Krishnan <nirenjan@nirenjan.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <stdlib.h>
#include <stdarg.h>

#include <vkm/vkm.h>

struct vkm_context {
    bool started;
    bool mouse_hi_res_scroll;
    bool mouse_horizontal_scroll;
};

vkm_result vkm_init(vkm_context **ctx)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    *ctx = calloc(1, sizeof(**ctx));
    if (*ctx == NULL) {
        return VKM_ERROR_OUT_OF_MEMORY;
    }

    return VKM_SUCCESS;
}

vkm_result vkm_reset(vkm_context *ctx)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (vkm_is_ready(ctx)) {
        return vkm_sync(ctx);
    }

    return VKM_SUCCESS;
}

void vkm_exit(vkm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    (void)vkm_reset(ctx);
    free(ctx);
}

vkm_result vkm_start(vkm_context *ctx)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    ctx->started = true;
    return VKM_SUCCESS;
}

bool vkm_is_ready(vkm_context *ctx)
{
    if (ctx == NULL) {
        return false;
    }

    return ctx->started;
}

bool vkm_platform_supported(void)
{
    return false;
}

bool vkm_feature_supported(vkm_feature feat)
{
    (void)feat;
    return false;
}

vkm_result vkm_set_option(vkm_context *ctx, vkm_option option, ...)
{
    va_list ap;
    bool flag;
    char *name;
    vkm_result rc;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    va_start(ap, option);
    rc = VKM_SUCCESS;
    switch (option) {
    case VKM_OPT_HI_RES_SCROLL:
        flag = (bool)va_arg(ap, int);
        ctx->mouse_hi_res_scroll = flag;
        break;

    case VKM_OPT_HORIZONTAL_SCROLL:
        flag = (bool)va_arg(ap, int);
        ctx->mouse_horizontal_scroll = flag;
        break;

    case VKM_OPT_DEVICE_NAME:
        name = va_arg(ap, char *);
        (void)name;
        rc = VKM_ERROR_NOT_SUPPORTED;
        break;

    default:
        rc = VKM_ERROR_INVALID_PARAM;
        break;
    }

    va_end(ap);
    return rc;
}

vkm_result vkm_mouse_move(vkm_context *ctx, int dx, int dy)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    if (dx == 0 && dy == 0) {
        return VKM_ERROR_NO_CHANGE;
    }

    return VKM_ERROR_NOT_SUPPORTED;
}

vkm_result vkm_mouse_click(vkm_context *ctx, vkm_mouse_button button, vkm_button_state state)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (button >= VKM_MOUSE_BTN_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (state >= VKM_BUTTON_STATE_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    return VKM_ERROR_NOT_SUPPORTED;
}

vkm_result vkm_mouse_scroll(vkm_context *ctx, vkm_mouse_scroll_direction dir)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (dir >= VKM_MOUSE_SCROLL_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    if (dir == VKM_MOUSE_SCROLL_LEFT || dir == VKM_MOUSE_SCROLL_RIGHT) {
        if (!ctx->mouse_horizontal_scroll) {
            return VKM_ERROR_NOT_SUPPORTED;
        }
    }

    return VKM_ERROR_NOT_SUPPORTED;
}

vkm_result vkm_sync(vkm_context *ctx)
{
    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    return VKM_SUCCESS;
}

vkm_result vkm_keyboard_send(vkm_context *ctx, vkm_key key, vkm_key_modifiers modifiers,
                             vkm_key_state state)
{
    (void)modifiers;

    if (ctx == NULL) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (key >= VKM_KEY_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (state >= VKM_KEY_STATE_MAX) {
        return VKM_ERROR_INVALID_PARAM;
    }

    if (!vkm_is_ready(ctx)) {
        return VKM_ERROR_NOT_READY;
    }

    return VKM_ERROR_NOT_SUPPORTED;
}
