#!/bin/bash
# Test setting LED brightness to 0x007d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x007d

$X52CLI bri LED 0x007d

verify_output

