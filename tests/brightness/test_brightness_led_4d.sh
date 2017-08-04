#!/bin/bash
# Test setting LED brightness to 0x004d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x004d

$X52CLI bri LED 0x004d

verify_output

