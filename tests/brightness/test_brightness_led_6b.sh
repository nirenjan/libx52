#!/bin/bash
# Test setting LED brightness to 0x006b

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x006b

$X52CLI bri LED 0x006b

verify_output

