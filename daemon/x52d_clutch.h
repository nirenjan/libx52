/*
 * Saitek X52 Pro MFD & LED driver - Clutch (profile selection UI)
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#ifndef X52D_CLUTCH_H
#define X52D_CLUTCH_H

#include "libx52io.h"

/**
 * @file x52d_clutch.h
 * @brief Clutch mode: profile selection on MFD (hat scroll, swipe left/right)
 */

#if defined(HAVE_EVDEV)

/**
 * @brief Process clutch button and, when engaged, clutch UI (hat/swipe).
 *
 * When ClutchEnabled and the clutch is engaged (latched toggle or held),
 * displays profile names on the MFD. Hat up/down scrolls; swipe left
 * clears (exit without change); swipe right selects the highlighted profile.
 *
 * @param report Current joystick report.
 * @param prev   Previous report (for edge detection).
 * @return true if clutch UI consumed the report (caller should skip profile/mouse).
 */
bool x52d_clutch_process(const libx52io_report *report,
                         const libx52io_report *prev);

#else

static inline bool x52d_clutch_process(const libx52io_report *report,
                                      const libx52io_report *prev) {
    (void)report;
    (void)prev;
    return false;
}

#endif /* HAVE_EVDEV */

#endif /* !defined X52D_CLUTCH_H */
