#!/bin/bash
# Test setting LED brightness to 0x006a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x006a

$X52CLI bri LED 0x006a

verify_output

