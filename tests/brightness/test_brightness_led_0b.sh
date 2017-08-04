#!/bin/bash
# Test setting LED brightness to 0x000b

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x000b

$X52CLI bri LED 0x000b

verify_output

