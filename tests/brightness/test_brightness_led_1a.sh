#!/bin/bash
# Test setting LED brightness to 0x001a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x001a

$X52CLI bri LED 0x001a

verify_output

