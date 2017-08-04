#!/bin/bash
# Test setting LED brightness to 0x0007

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0007

$X52CLI bri LED 0x0007

verify_output

