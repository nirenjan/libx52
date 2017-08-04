#!/bin/bash
# Test setting LED brightness to 0x0059

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0059

$X52CLI bri LED 0x0059

verify_output

