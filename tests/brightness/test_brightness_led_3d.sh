#!/bin/bash
# Test setting LED brightness to 0x003d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x003d

$X52CLI bri LED 0x003d

verify_output

