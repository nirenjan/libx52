Virtual keyboard/mouse infrastructure
=====================================

The virtual keyboard/mouse infrastructure (or VKM), is an API used by an
application to inject keyboard and mouse events into the OS stack. The advantage
of using a separate API for this is so that this can be handled in a
cross-platform manner without having to sprinkle `#ifdef`'s throughout the
program.

Base API
========

The API is based around a context, which is an opaque pointer returned by
`vkm_init`. All subsequent VKM calls will take in this pointer as the first
argument, and return a signed 32-bit status code. Once done, `vkm_exit` will
clean up any data structures and close any file descriptors that were opened as
part of the VKM calls.

VKM can also be configured through the `vkm_config` API call.

Device handling
===============

`vkm_new_device` is the API to use when creating a new VKM device. While VKM
will support both keyboard and mouse events from a single device, there may be
cases where the application needs to separate out keyboard and mouse events into
different devices. The flags will enable keyboard and/or mouse support.

Note that the supported event codes (on Linux) are fixed, and cannot be updated.
The keyboard will emulate a standard US keyboard, while the mouse will emulate a
standard 3 button mouse with a scroll wheel.

Mouse handling
==============

The mouse is handled as a single API call that passes in dx and dy to move the
mouse in the relative X and Y axes. VKM will take care of internally translating
the calls to the appropriate framework. This is handled in `vkm_mouse_move`

The scroll wheel is handled through `vkm_mouse_scroll`. By default, the mouse
motion uses standard scrolling, but high resolution scrolling may be enabled.

The buttons are handled by `vkm_mouse_click`. The API will send the state,
depending on the input (pressed or not)

Keyboard handling
=================

The keyboard is handled through a single call `vkm_keyboard_send`. This sends a
single key event, with modifiers enabled (Ctrl, Shift, Alt, GUI).
