/*
 * Saitek X52 Pro MFD & LED driver - Configuration parser
 *
 * Copyright (C) 2021 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include "config.h"
#include "pinelog.h"
#include "x52d_config.h"
#include "x52d_const.h"

static struct x52d_config x52d_config;

void x52d_config_load(const char *cfg_file)
{
    int rc;

    if (cfg_file == NULL) {
        cfg_file = X52D_SYS_CFG_FILE;
    }

    rc = x52d_config_set_defaults(&x52d_config);
    if (rc != 0) {
        PINELOG_FATAL(_("Error %d setting configuration defaults: %s"),
                      rc, strerror(rc));
    }

    rc = x52d_config_load_file(&x52d_config, cfg_file);
    if (rc != 0) {
        exit(EXIT_FAILURE);
    }

    // Apply overrides
    rc = x52d_config_apply_overrides(&x52d_config);
    x52d_config_clear_overrides();
    if (rc != 0) {
        exit(EXIT_FAILURE);
    }
}

/* Callback stubs
 * TODO: Remove the ones below when their implementation is complete
 */
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
void x52d_cfg_set_Brightness_MFD(uint16_t param) { (void)param; }
void x52d_cfg_set_Brightness_LED(uint16_t param) { (void)param; }
void x52d_cfg_set_Profiles_Directory(char* param) { (void)param; }
void x52d_cfg_set_Profiles_ClutchEnabled(bool param) { (void)param; }
void x52d_cfg_set_Profiles_ClutchLatched(bool param) { (void)param; }

void x52d_config_apply(void)
{
    #define CFG(section, key, name, parser, def) \
        PINELOG_TRACE("Calling configuration callback for " #section "." #name); \
        x52d_cfg_set_ ## section ## _ ## key(x52d_config . name);
    #include "x52d_config.def"
}
