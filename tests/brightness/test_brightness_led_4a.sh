#!/bin/bash
# Test setting LED brightness to 0x004a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x004a

$X52CLI bri LED 0x004a

verify_output

