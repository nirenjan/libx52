#!/bin/bash
# Test setting LED brightness to 0x0058

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0058

$X52CLI bri LED 0x0058

verify_output

