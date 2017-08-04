#!/bin/bash
# Test setting LED brightness to 0x0002

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0002

$X52CLI bri LED 0x0002

verify_output

