#!/bin/bash
# Test setting LED brightness to 0x0039

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0039

$X52CLI bri LED 0x0039

verify_output

