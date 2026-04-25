/*
 * Saitek X52 Pro MFD line scrolling (libx52util)
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libx52/libx52util.h>

#define SCROLL_OUT_BUF 256u
#define SCROLL_WINDOW 16u
#define SCROLL_PAD 16u

struct libx52util_scroll {
    uint8_t text[SCROLL_OUT_BUF];
    size_t text_len;
    uint32_t flags;
    size_t prefix;
    size_t suffix;
    size_t tape_len;
    size_t max_pos;
    size_t pos;
    bool has_last;
    uint8_t last[SCROLL_WINDOW];
    bool static_mode;
    bool static_emitted;
    bool single_pass;
    bool single_pass_done;
    bool ltr;
};

static uint8_t tape_byte(const struct libx52util_scroll *st, size_t index)
{
    if (index < st->prefix) {
        return (uint8_t)' ';
    }
    index -= st->prefix;
    if (index < st->text_len) {
        return st->text[index];
    }
    index -= st->text_len;
    if (index < st->suffix) {
        return (uint8_t)' ';
    }
    return (uint8_t)' ';
}

static void fill_frame(const struct libx52util_scroll *st, size_t tape_pos,
                       uint8_t display[SCROLL_WINDOW])
{
    for (size_t i = 0; i < SCROLL_WINDOW; i++) {
        size_t idx = tape_pos + i;
        if (idx < st->tape_len) {
            display[i] = tape_byte(st, idx);
        } else {
            display[i] = (uint8_t)' ';
        }
    }
}

static void advance_pos(struct libx52util_scroll *st)
{
    if (st->static_mode) {
        return;
    }
    if (st->single_pass_done) {
        return;
    }

    if (!st->ltr) {
        if (st->pos < st->max_pos) {
            st->pos++;
        } else if (!st->single_pass) {
            st->pos = 0;
        } else {
            st->single_pass_done = true;
        }
    } else {
        if (st->pos > 0) {
            st->pos--;
        } else if (!st->single_pass) {
            st->pos = st->max_pos;
        } else {
            st->single_pass_done = true;
        }
    }
}

int libx52util_scroll_new(libx52util_scroll_state **state,
                          const uint8_t *utf8_string, libx52util_scroll_flags flags)
{
    struct libx52util_scroll *st;
    size_t out_len;
    int conv_rc;

    if (!state || !utf8_string) {
        return -EINVAL;
    }

    st = calloc(1, sizeof(*st));
    if (!st) {
        return -ENOMEM;
    }

    out_len = SCROLL_OUT_BUF;
    conv_rc = libx52util_convert_utf8_string(utf8_string, st->text, &out_len);
    if (conv_rc == -EINVAL) {
        free(st);
        return -EINVAL;
    }

    st->text_len = out_len;
    st->flags = (uint32_t)flags;

    {
        uint32_t eff = st->flags;
        if (eff & (uint32_t)LIBX52UTIL_SCROLL_SINGLE_PASS) {
            eff |= (uint32_t)LIBX52UTIL_SCROLL_OUT;
        }
        st->prefix = (eff & (uint32_t)LIBX52UTIL_SCROLL_IN) ? SCROLL_PAD : 0;
        st->suffix = (eff & (uint32_t)LIBX52UTIL_SCROLL_OUT) ? SCROLL_PAD : 0;
    }

    st->tape_len = st->prefix + st->text_len + st->suffix;
    st->ltr = (st->flags & (uint32_t)LIBX52UTIL_SCROLL_LTR) != 0;
    st->single_pass = (st->flags & (uint32_t)LIBX52UTIL_SCROLL_SINGLE_PASS) != 0;

    st->static_mode =
        (st->text_len <= SCROLL_WINDOW) || (st->tape_len <= SCROLL_WINDOW);

    if (st->tape_len > SCROLL_WINDOW) {
        st->max_pos = st->tape_len - SCROLL_WINDOW;
    } else {
        st->max_pos = 0;
    }

    st->pos = st->ltr ? st->max_pos : 0;
    st->has_last = false;
    st->static_emitted = false;
    st->single_pass_done = false;

    *state = st;

    if (conv_rc == -E2BIG) {
        return -E2BIG;
    }
    return 0;
}

void libx52util_scroll_free(libx52util_scroll_state **state)
{
    if (!state || !*state) {
        return;
    }
    free(*state);
    *state = NULL;
}

int libx52util_scroll_reset(libx52util_scroll_state *state)
{
    if (!state) {
        return -EINVAL;
    }

    state->pos = state->ltr ? state->max_pos : 0;
    state->has_last = false;
    state->static_emitted = false;
    state->single_pass_done = false;
    return 0;
}

int libx52util_scroll_next(libx52util_scroll_state *state, uint8_t display[SCROLL_WINDOW])
{
    uint8_t candidate[SCROLL_WINDOW];

    if (!state || !display) {
        return -EINVAL;
    }

    if (state->static_mode && state->static_emitted) {
        return -EAGAIN;
    }

    fill_frame(state, state->pos, candidate);

    if (state->has_last &&
        memcmp(candidate, state->last, SCROLL_WINDOW) == 0) {
        advance_pos(state);
        return -EAGAIN;
    }

    memcpy(display, candidate, SCROLL_WINDOW);
    memcpy(state->last, candidate, SCROLL_WINDOW);
    state->has_last = true;

    if (state->static_mode) {
        state->static_emitted = true;
    }

    advance_pos(state);
    return 0;
}
