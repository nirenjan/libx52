#!/bin/bash
# Test setting LED brightness to 0x005d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x005d

$X52CLI bri LED 0x005d

verify_output

