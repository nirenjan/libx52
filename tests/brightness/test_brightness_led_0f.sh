#!/bin/bash
# Test setting LED brightness to 0x000f

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x000f

$X52CLI bri LED 0x000f

verify_output

