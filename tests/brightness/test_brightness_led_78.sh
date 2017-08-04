#!/bin/bash
# Test setting LED brightness to 0x0078

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0078

$X52CLI bri LED 0x0078

verify_output

