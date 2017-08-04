#!/bin/bash
# Test setting LED brightness to 0x0069

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0069

$X52CLI bri LED 0x0069

verify_output

