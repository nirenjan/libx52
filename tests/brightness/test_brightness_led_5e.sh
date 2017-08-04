#!/bin/bash
# Test setting LED brightness to 0x005e

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x005e

$X52CLI bri LED 0x005e

verify_output

