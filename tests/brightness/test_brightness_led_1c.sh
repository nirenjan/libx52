#!/bin/bash
# Test setting LED brightness to 0x001c

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x001c

$X52CLI bri LED 0x001c

verify_output

