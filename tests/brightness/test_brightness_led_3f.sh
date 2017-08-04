#!/bin/bash
# Test setting LED brightness to 0x003f

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x003f

$X52CLI bri LED 0x003f

verify_output

