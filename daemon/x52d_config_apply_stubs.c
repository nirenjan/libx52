/*
 * Saitek X52 Pro MFD & LED driver - No-op config apply stubs for unit tests
 *
 * When linking the profile test we use x52d_config.c but not the full daemon
 * (clock, led, mouse). x52d_config_apply() and x52d_config_apply_immediate()
 * call every x52d_cfg_set_* from x52d_config.def. This file provides no-op
 * implementations for Clock, LED, Brightness, and Mouse so the test links.
 * Profiles_* are already defined as no-ops in x52d_config.c.
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "x52d_config.h"

/* Clock */
void x52d_cfg_set_Clock_Enabled(bool param) { (void)param; }
void x52d_cfg_set_Clock_PrimaryIsLocal(bool param) { (void)param; }
void x52d_cfg_set_Clock_Secondary(char *param) { (void)param; }
void x52d_cfg_set_Clock_Tertiary(char *param) { (void)param; }
void x52d_cfg_set_Clock_FormatPrimary(libx52_clock_format param) { (void)param; }
void x52d_cfg_set_Clock_FormatSecondary(libx52_clock_format param) { (void)param; }
void x52d_cfg_set_Clock_FormatTertiary(libx52_clock_format param) { (void)param; }
void x52d_cfg_set_Clock_DateFormat(libx52_date_format param) { (void)param; }

/* LED */
void x52d_cfg_set_LED_Fire(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_Throttle(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_A(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_B(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_D(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_E(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_T1(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_T2(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_T3(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_POV(libx52_led_state param) { (void)param; }
void x52d_cfg_set_LED_Clutch(libx52_led_state param) { (void)param; }

/* Brightness */
void x52d_cfg_set_Brightness_MFD(uint16_t param) { (void)param; }
void x52d_cfg_set_Brightness_LED(uint16_t param) { (void)param; }

/* Mouse */
void x52d_cfg_set_Mouse_Enabled(bool param) { (void)param; }
void x52d_cfg_set_Mouse_Speed(int param) { (void)param; }
void x52d_cfg_set_Mouse_ReverseScroll(bool param) { (void)param; }
