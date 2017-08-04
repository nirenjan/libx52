#!/bin/bash
# Test setting LED brightness to 0x0067

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0067

$X52CLI bri LED 0x0067

verify_output

