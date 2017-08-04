#!/bin/bash
# Test setting LED brightness to 0x0041

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0041

$X52CLI bri LED 0x0041

verify_output

