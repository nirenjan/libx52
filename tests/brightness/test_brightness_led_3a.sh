#!/bin/bash
# Test setting LED brightness to 0x003a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x003a

$X52CLI bri LED 0x003a

verify_output

