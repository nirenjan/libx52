#!/bin/bash
# Test setting LED brightness to 0x000e

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x000e

$X52CLI bri LED 0x000e

verify_output

